#include "pch.h"
#include "jester_reqrep.h"

using namespace std;
using namespace std::placeholders;
using namespace bricks;
using namespace bricks::plugins;

BRICKSJESTER_API brick_t*
bricks::plugins::create_jester_reqrep_ctx()
{
	return new jester_server_ctx_t();
}

BRICKSJESTER_API server_plugin_t*
bricks::plugins::create_jester_server(brick_t* ctx)
{
	return new jester_server_t((jester_server_ctx_t*)ctx);
}

BRICKSJESTER_API client_plugin_t*
bricks::plugins::create_jester_client(brick_t* ctx)
{
	return new jester_client_t((jester_server_ctx_t*)ctx);
}

jester_server_ctx_t::jester_server_ctx_t()
{

}

void
jester_server_ctx_t::set_server(jester_server_t* server)
{
	this->server = server;
}

bricks_error_code_e 
jester_server_ctx_t::issue_request(response_proxy_cb_t proxy, const char* data, size_t size)
{
	return this->server->issue_request(proxy, data, size);
}


jester_server_t::jester_server_t(jester_server_ctx_t* ctx)
{
	this->ctx = ctx;
	ctx->set_server(this);
}


bricks_error_code_e 
jester_server_t::init(cb_queue_t* queue, request_cb_t request_cb, const xtree_t*)
{
	SYNCHRONIZED(ctx->mtx);

	ASSERT_NOT_INITIATED;

	this->queue = queue;

	this->initiated = true;

	this->request_cb = request_cb;

	return BRICKS_SUCCESS;
}



bricks_error_code_e 
jester_server_t::issue_request(response_proxy_cb_t proxy, const char* data, size_t size)
{
	SYNCHRONIZED(ctx->mtx);

	ASSERT_INITIATED;
	ASSERT_STARTED;

	auto buf = create_buffer(data, size);
	auto xt  = create_xtree();

	callback_t cb = std::bind(
		request_cb,
		proxy,
		buf,
		xt);

	queue->enqueue(cb);
	
	return BRICKS_SUCCESS;
}

bricks_error_code_e 
jester_server_t::response_proxy(response_proxy_cb_t proxy, bricks_error_code_e e, const char* data, size_t size, xtree_t* xt)
{
	SYNCHRONIZED(ctx->mtx);

	ASSERT_INITIATED;
	ASSERT_STARTED;

	proxy(e, data, size, xt);

	return BRICKS_SUCCESS;
}

bool 
jester_server_t::check_capability(plugin_capabilities_e)
{
	SYNCHRONIZED(ctx->mtx);

	return false;
}

jester_client_t::jester_client_t(jester_server_ctx_t* ctx) :ctx(ctx)
{

}

bool
jester_client_t::check_capability(plugin_capabilities_e)
{
	SYNCHRONIZED(ctx->mtx);

	return false;
}

bricks_error_code_e 
jester_client_t::init(cb_queue_t* queue, int timeout_ms, const xtree_t* options)
{
	SYNCHRONIZED(ctx->mtx);

	ASSERT_NOT_INITIATED;

	this->queue = queue;

	this->timeout_ms = timeout_ms;

	this->initiated = true;

	return BRICKS_SUCCESS;
}

bricks_error_code_e
jester_client_t::issue_request(const char* data, size_t size, response_cb_t response_cb, const xtree_t* options)
{
	SYNCHRONIZED(ctx->mtx);

	ASSERT_INITIATED;
	ASSERT_STARTED;
	
	response_proxy_cb_t proxy = std::bind(&jester_client_t::client_response_proxy, this, response_cb, _1, _2, _3, _4);

	ctx->issue_request(proxy, data, size);
	
	return BRICKS_SUCCESS;
}

bricks_error_code_e 
jester_client_t::client_response_proxy(response_cb_t response_cb, bricks_error_code_e err, const char* data, size_t size, xtree_t* xt)
{
	SYNCHRONIZED(ctx->mtx);

	auto buf = create_buffer(data, size);

	
	callback_t cb = std::bind(response_cb, err, buf, xt);

	queue->enqueue(cb);
	
	
	return BRICKS_SUCCESS;

}

