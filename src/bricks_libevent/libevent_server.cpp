#include "pch.h"
#include "libevent_server.h"
#include "libevent_plugin.h"

using namespace std;
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
libevent_server_t::init(cb_queue_t* queue, const xtree_t* options)
{
	return BRICKS_SUCCESS;
}

bricks_error_code_e 
libevent_server_t::register_request_cb(request_cb_t request, const xtree_t* options)
{
	return BRICKS_SUCCESS;
}


libevent_server_t::~libevent_server_t()
{

}

