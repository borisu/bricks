#include "pch.h"
#include "libevent_server.h"
#include "libevent_plugin.h"

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

	name = get_opt<string>(options->get_property_value("/bricks/libevent/server", "name"));

	bool debug = get_opt<bool>(options->get_property_value("/bricks/libevent/server", "debug"), false);

	string address = options ? get_opt<string>(options->get_property_value("/bricks/libevent/server/init/connection", "address"), "0.0.0.0") : "0.0.0.0";

	int port = options ? get_opt<int>(options->get_property_value("/bricks/libevent/server/init/connection", "port"), 8080) : 8080;

	this->request = request;

	cfg = event_config_new();

	base = event_base_new_with_config(cfg);

	http = evhttp_new(base);

	if (debug)
		event_enable_debug_logging(EVENT_DBG_ALL);

	//evhttp_set_cb(http, uri.c_str(), request_proxy, c);

	handle = evhttp_bind_socket_with_handle(http, address.c_str(), port);

	libevent_poll_thread = new thread(&libevent_server_t::libevent_poll_loop, this);

	initiated = true;

	if (!handle) {
		//fprintf(stderr, "Could not bind to port 8080.\n");
		//return 1;
	}

	return BRICKS_SUCCESS;
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

	/*if (term)
		event_free(term);*/

	if (base)
		event_base_free(base);

	
}

void
libevent_server_t::libevent_poll_loop(void)
{

}

libevent_server_t::~libevent_server_t()
{
	SYNCHRONIZED(mtx);

	destroy();

}

