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
jester_server_ctx_t::issue_request(server_proxy_cb_t proxy, const char* data, size_t size)
{
	return this->server->issue_request(proxy, data, size);
}


jester_server_t::jester_server_t(jester_server_ctx_t* ctx)
{
	this->ctx = ctx;
	ctx->set_server(this);
}


bricks_error_code_e 
jester_server_t::init(cb_queue_t* queue, const xtree_t*)
{
	this->queue = queue;

	return BRICKS_SUCCESS;
}

bricks_error_code_e 
jester_server_t::register_request_handler(server_request_cb_t request_cb, const xtree_t* options)
{
	this->request_cb = request_cb;
	return BRICKS_SUCCESS;
}

bricks_error_code_e 
jester_server_t::start()
{
	return BRICKS_SUCCESS;
}

bricks_error_code_e 
jester_server_t::issue_request(server_proxy_cb_t proxy, const char* data, size_t size)
{
	auto buf = create_buffer(data, size);
	auto xt  = create_xtree();

	callback_t cb = std::bind(
		request_cb,
		proxy,
		buf,
		xt);

	if (queue)
	{
		queue->enqueue(cb);
	}
	else
	{
		request_cb(
			std::bind(&jester_server_t::server_response_proxy, this, proxy, _1, _2, _3, _4),
			create_buffer(data, size),
			create_xtree());
	}
	
	return BRICKS_SUCCESS;
}

bricks_error_code_e 
jester_server_t::server_response_proxy(server_proxy_cb_t proxy, bricks_error_code_e e, const char* data, size_t size, xtree_t* xt)
{
	proxy(e, data, size, xt);

	return BRICKS_SUCCESS;
}


jester_client_t::jester_client_t(jester_server_ctx_t* ctx) :ctx(ctx)
{

}

bricks_error_code_e 
jester_client_t::init(cb_queue_t* queue, const xtree_t* options)
{
	this->queue = queue;

	return BRICKS_SUCCESS;
}

bricks_error_code_e
jester_client_t::request(const char* data, size_t size, client_response_cb_t response_cb, const xtree_t* options)
{
	
	server_proxy_cb_t proxy = std::bind(&jester_client_t::client_response_proxy, this, response_cb, _1, _2, _3, _4);

	ctx->issue_request(proxy, data, size);
	
	return BRICKS_SUCCESS;
}

bricks_error_code_e 
jester_client_t::client_response_proxy(client_response_cb_t response_cb, bricks_error_code_e err, const char* data, size_t size, xtree_t* xt)
{
	auto buf = create_buffer(data, size);

	if (queue)
	{
		callback_t cb = std::bind(response_cb, err, buf, xt);

		queue->enqueue(cb);
	}
	else
	{
		response_cb(err, buf, xt);
	}

	return BRICKS_SUCCESS;

}

bricks_error_code_e 
jester_client_t::start()
{
	return BRICKS_SUCCESS;
}
