#include "pch.h"
#include "libevent_plugin.h"
#include "libevent_client.h"
#include "libevent_base.h"

using namespace std;
using namespace bricks;
using namespace bricks::plugins;

client_plugin_t*
bricks::plugins::create_libevent_client()
{
	return new libevent_client_t();
}

libevent_client_t::libevent_client_t()
{

}

libevent_client_t::~libevent_client_t()
{

}

void 
libevent_client_t::set_meta_cb(meta_cb_t meta_cb)
{
	this->meta_cb = meta_cb;
}

bool 
libevent_client_t::check_capability(plugin_capabilities_e)
{
	return false;
}

void
libevent_client_t::destroy()
{
	destroyed = true;

	shutdown = true;

	if (libevent_poll_thread)
		libevent_poll_thread->join();

	if (cfg)
		event_config_free(cfg);

	if (term)
		event_free(term);

	if (base)
		event_base_free(base);
}

void
libevent_client_t::libevent_poll_loop()
{
	struct timeval ten_sec;

	ten_sec.tv_sec = 1;
	ten_sec.tv_usec = 0;

	/* Now we run the event_base for a series of 10-second intervals, printing
	   "Tick" after each.  For a much better way to implement a 10-second
	   timer, see the section below about persistent timer events. */
	while (!shutdown) {
		/* This schedules an exit ten seconds from now. */
		event_base_loopexit(base, &ten_sec);
		event_base_dispatch(base);
	}

}

void
libevent_client_t::do_term(evutil_socket_t sig, short events, void* arg)
{
	((libevent_client_t*)arg)->shutdown = true;
	event_base_loopbreak(((libevent_client_t*)arg)->base);
}

bricks_error_code_e
libevent_client_t::init(cb_queue_t* queue, int timeout_ms, const xtree_t* options)
{
	SYNCHRONIZED(mtx);

	ASSERT_PREINIT;

	name = get_opt<string>(options->get_property_value("/bricks/libevent/client", "name"));

	this->timeout_ms = timeout_ms;

	this->queue = queue;

	base = event_base_new();
	if (!base) {
		log1(BRICKS_ALARM, "%s %%%%%% Couldn't create an event_base (event_base_new).", this->name.c_str());
		destroy();
		return BRICKS_3RD_PARTY_ERROR;
	}

	term = event_new(base, -1, EV_PERSIST | EV_READ, do_term, NULL);
	auto rc = event_add(term, NULL);

	libevent_poll_thread = new thread(&libevent_client_t::libevent_poll_loop, this);

	initiated = true;

	return BRICKS_SUCCESS;
}

bricks_error_code_e 
libevent_client_t::issue_request(const char* data, size_t size, response_cb_t response_cb, const xtree_t* options)
{
	SYNCHRONIZED(mtx);

	ASSERT_READY;

	string address = options ? get_opt<string>(options->get_property_value("/bricks/libevent/client/methods/issue_request/http", "address"), "0.0.0.0") : "0.0.0.0";

	int port = options ? get_opt<int>(options->get_property_value("/bricks/client/methods/issue_request/http", "port"), 8080) : 8080;

	string method = options ? get_opt<string>(options->get_property_value("/bricks/client/methods/issue_request/http", "method"), "GET") : "GET";
	
	string path = options ? get_opt<string>(options->get_property_value("/bricks/libevent/client/methods/issue_request/http", "path")) : "/api";
	
	auto ctx = new req_ctx{};
	ctx->This = this;
	ctx->counter = ++counter;

	ctx->conn = evhttp_connection_base_new(base, NULL, address.c_str(), port);
	if (!ctx->conn) {
		log1(BRICKS_ALARM, "%s %%%%%% Could not create connection to %s:%d (evhttp_connection_base_new).", this->name.c_str(), address.c_str(), port);
		return BRICKS_3RD_PARTY_ERROR;;
	}
	
	ctx->req = evhttp_request_new(http_request_done, (void*)ctx);
	if (!ctx->req) {
		log1(BRICKS_ALARM, "%s %%%%%% Could not create connection to (evhttp_request_new).", this->name.c_str());
		evhttp_connection_free(ctx->conn);
		delete ctx;
		return BRICKS_3RD_PARTY_ERROR;
	}

	evhttp_add_header(evhttp_request_get_output_headers(ctx->req), "Host", address.c_str());
	if (-1 == evhttp_make_request(ctx->conn, ctx->req, method == "POST" ? EVHTTP_REQ_POST : EVHTTP_REQ_GET, path.c_str()))
	{
		log1(BRICKS_ALARM, "%s %%%%%% Error making request (evhttp_make_request).", this->name.c_str());
		evhttp_request_free(ctx->req);
		evhttp_connection_free(ctx->conn);
		delete ctx;
		return BRICKS_3RD_PARTY_ERROR;
	}

	
	evbuffer_add(evhttp_request_get_output_buffer(ctx->req), data, size);

	// Set a timeout for the request
	timeval tv = { 0, timeout_ms*1000 };  // 5 seconds timeout
	ctx->timeout_event = evtimer_new(base, timeout_cb, (void*)ctx);
	evtimer_add(ctx->timeout_event, &tv);

	ctx->response_cb = response_cb;

	ctxs[counter] = ctx;

	return BRICKS_SUCCESS;
}


void 
libevent_client_t::http_request_done(evhttp_request* req, void* arg)
{
	req_ctx* ctx = (req_ctx*)arg;

	SYNCHRONIZED(ctx->This->mtx);

	ctx->This->ctxs.erase(ctx->counter);

	event_del(ctx->timeout_event);

	evhttp_connection_free(ctx->conn);

	auto evbuf = evhttp_request_get_input_buffer(req);

	auto buf = create_buffer((char*)evbuffer_pullup(evbuf, -1), evbuffer_get_length(evbuf));

	auto response_xt = create_xtree();
	
	ctx->This->queue->enqueue(
		std::bind(ctx->response_cb, evhttp_request_get_response_code(req) == 200 ? BRICKS_SUCCESS : BRICKS_REMOTE_ERROR, buf, response_xt));

	delete ctx;
	
}

void 
libevent_client_t::timeout_cb(evutil_socket_t fd, short what, void* arg)
{
	req_ctx* ctx = (req_ctx*)arg;

	SYNCHRONIZED(ctx->This->mtx);

	evhttp_cancel_request(ctx->req);

//	evhttp_request_free(ctx->req);

	evhttp_connection_free(ctx->conn);

	ctx->This->queue->enqueue(std::bind(ctx->response_cb,BRICKS_TIMEOUT, nullptr, nullptr));

	ctx->This->ctxs.erase(ctx->counter);

	delete ctx;
	
}