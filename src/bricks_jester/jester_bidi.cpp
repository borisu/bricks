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
	SYNCHRONIZED(ctx->mtx);

	ctx->set_endpoint(id, this);
}

bricks_error_code_e 
jester_bidi_t::init(cb_queue_t* queue, event_cb_t event_cb, const xtree_t* options)
{
	SYNCHRONIZED(ctx->mtx);

	ASSERT_PREINIT;

	this->queue = queue;

	this->event_cb = event_cb;

	initiated = true;

	return BRICKS_SUCCESS;
}


bricks_error_code_e 
jester_bidi_t::accept_event(const char* data, size_t size)
{
	SYNCHRONIZED(ctx->mtx);

	return this->queue->enqueue(std::bind(this->event_cb, create_buffer(data, size), nullptr));
}


bricks_error_code_e 
jester_bidi_t::send_event(const char* data, size_t size, const xtree_t* options)
{
	SYNCHRONIZED(ctx->mtx);

	ASSERT_READY;

	return this->ctx->send_event(this->id == 0 ? 1 : 0, data, size);
}

bool
jester_bidi_t::check_capability(plugin_capabilities_e cap)
{
	SYNCHRONIZED(ctx->mtx);

	return false;
}