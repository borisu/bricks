#include "pch.h"
#include "libevent_plugin.h"
#include "libevent_client.h"

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

bricks_error_code_e
libevent_client_t::init(cb_queue_t* queue, int timeout_ms, const xtree_t* options)
{
	return BRICKS_SUCCESS;
}

bricks_error_code_e libevent_client_t::issue_request(const char*, size_t, response_cb_t, const xtree_t* options)
{
	return BRICKS_SUCCESS;
}
