#include "pch.h"
#include "jester_bidi.h"

using namespace bricks;
using namespace bricks::plugins;

brick_t*
bricks::plugins::create_jester_bidi_context()
{
	return new jester_bidi_ctx_t();
}

bidi_plugin_t*
bricks::plugins::create_jester_bidi(brick_t* ctx, int id)
{
	return new jester_bidi_t((jester_bidi_ctx_t*)ctx, id);
}

jester_bidi_ctx_t::jester_bidi_ctx_t()
{

}

bricks_error_code_e
jester_bidi_ctx_t::set_endpoint(int source, jester_bidi_t* j)
{
	if (source != 0 && source != 1)
		return BRICKS_INVALID_PARAM;

	endpoints[source] = j;

	return BRICKS_SUCCESS;
}


bricks_error_code_e 
jester_bidi_ctx_t::send_event(int source, const char* data, size_t size)
{
	auto it = endpoints.find(source);
	if (it == endpoints.end())
		return BRICKS_INVALID_PARAM;

	endpoints[source]->accept_event(data, size);

	return BRICKS_SUCCESS;
}

jester_bidi_t::jester_bidi_t(jester_bidi_ctx_t* ctx, int id):ctx(ctx),id(id)
{
	ctx->set_endpoint(id, this);
}

bricks_error_code_e 
jester_bidi_t::init(cb_queue_t* queue, const xtree_t* options)
{
	this->queue = queue;
	return BRICKS_SUCCESS;
}

bricks_error_code_e 
jester_bidi_t::register_event_handler(event_cb_t event_cb, const xtree_t* options)
{
	this->event_cb = event_cb;
	return BRICKS_SUCCESS;
}

bricks_error_code_e 
jester_bidi_t::accept_event(const char* data, size_t size)
{
	if (this->queue)
	{
		this->queue->enqueue(std::bind(this->event_cb, create_buffer(data, size), nullptr));
	}
	else
	{
		this->event_cb(create_buffer(data, size), nullptr);
	}
	return BRICKS_SUCCESS;
}

bricks_error_code_e 
jester_bidi_t::start()
{
	return BRICKS_SUCCESS;
}

bricks_error_code_e 
jester_bidi_t::send_event(const char* data, size_t size, const xtree_t* options)
{
	return this->ctx->send_event(this->id == 0 ? 1 : 0, data, size);
}