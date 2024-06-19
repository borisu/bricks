#include "pch.h"
#include "libevent_server.h"
#include "libevent_plugin.h"
#include "libevent_base.h"

using namespace std;
using namespace std::placeholders;
using namespace bricks;
using namespace bricks::plugins;




server_plugin_t*
bricks::plugins::create_libevent_server()
{
	return new libevent_server_t();
}

bool
libevent_server_t::check_capability(plugin_capabilities_e)
{
	return false;
}

libevent_server_t::libevent_server_t()
{

}

bricks_error_code_e 
libevent_server_t::init(cb_queue_t* queue, request_cb_t request, const xtree_t* options)
{
	SYNCHRONIZED(mtx);

	this->queue = queue;

	name = get_opt<string>(options->get_property_value("/bricks/libevent/server", "name"));

	string address = options ? get_opt<string>(options->get_property_value("/bricks/libevent/server/methods/init/connection", "address"), "0.0.0.0") : "0.0.0.0";

	int port = options ? get_opt<int>(options->get_property_value("/bricks/libevent/server/methods/init/connection", "port"), 8080) : 8080;

	this->request = request;

	cfg = event_config_new();

	base = event_base_new_with_config(cfg);

	if (!base) {
		log1(BRICKS_ALARM, "%s %%%%%% Couldn't create an event_base (event_base_new_with_config).", this->name.c_str());
		destroy();
		return BRICKS_3RD_PARTY_ERROR;
	}

	event_config_free(cfg);
	cfg = nullptr;

	http = evhttp_new(base);
	if (!http) {
		log1(BRICKS_ALARM, "%s %%%%%% Couldn't create evhttp.", this->name.c_str());
		destroy();
		return BRICKS_3RD_PARTY_ERROR;
	}

	term = event_new(base, -1, EV_PERSIST | EV_READ, do_term, NULL);
	auto rc = event_add(term, NULL);
	
	evhttp_set_gencb(http, do_handle_request,this);

	handle = evhttp_bind_socket_with_handle(http, address.c_str(), port);

	if (!handle) {
		log1(BRICKS_ALARM, "%s %%%%%% Couldn't bind to %s:%d.", this->name.c_str(), address.c_str(), port);
		destroy();
		return BRICKS_3RD_PARTY_ERROR;
	}

	libevent_poll_thread = new thread(&libevent_server_t::libevent_poll_loop, this);

	initiated = true;

	return BRICKS_SUCCESS;
}

void libevent_server_t::do_handle_request(struct evhttp_request* req, void* arg)
{
	libevent_server_t * const This = (libevent_server_t*)(arg);

	SYNCHRONIZED(This->mtx);

	struct evbuffer* buf = evhttp_request_get_input_buffer(req);

	response_proxy_cb_t  f =
		std::bind(&libevent_server_t::do_handle_response, This, req, _1, _2, _3, _4);

	auto xt = create_xtree();

	auto hnode = xt->add_node("/bricks/libevent/server/request", true);
	xt->set_property_value(hnode.value(), "uri", evhttp_request_get_uri(req));

	This->queue->enqueue(
		std::bind(This->request, f, create_buffer((char*)evbuffer_pullup(buf, -1), evbuffer_get_length(buf)), nullptr));

};

void 
libevent_server_t::do_handle_response(struct evhttp_request* req, bricks_error_code_e err, const char* data, size_t size, xtree_t*)
{
	SYNCHRONIZED(mtx);

	evhttp_add_header(evhttp_request_get_output_headers(req),
		"Content-Type", "text/plain");

	auto evb = evbuffer_new();

	evbuffer_add(evb, data, strlen(data));

	evhttp_send_reply(req,err == BRICKS_SUCCESS ? 200 : 500, err == BRICKS_SUCCESS? "OK" : "Internal Server Error", evb);

	if (evb)
		evbuffer_free(evb);
}

void 
libevent_server_t::do_term(evutil_socket_t sig, short events, void* arg)
{
	((libevent_server_t*)arg)->shutdown = true;
	event_base_loopbreak(((libevent_server_t*)arg)->base);
}

void
libevent_server_t::libevent_poll_loop()
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


void libevent_server_t::destroy()
{
	destroyed = true;

	shutdown = true;

	if (libevent_poll_thread)
		libevent_poll_thread->join();

	if (cfg)
		event_config_free(cfg);

	if (http)
		evhttp_free(http);

	if (term)
		event_free(term);

	if (base)
		event_base_free(base);
}

libevent_server_t::~libevent_server_t()
{
	SYNCHRONIZED(mtx);

	destroy();

}

