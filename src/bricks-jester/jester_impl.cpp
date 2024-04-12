#include "pch.h"
#include "utils.h"
#include "jester_impl.h"

using namespace std;
using namespace bricks;
using namespace bricks::plugins;


brick_t*
bricks::plugins::create_jester_context()
{
	return new jester_ctx_t();
}

publisher_plugin_t*
bricks::plugins::create_jester_publisher(brick_t* ctx)
{
	return new jester_publisher_t((jester_ctx_t*)ctx);
}

BRICKSJESTER_API subscriber_plugin_t*
bricks::plugins::create_jester_subscriber(brick_t* ctx)
{
	return new jester_subscriber_t((jester_ctx_t*)ctx);
}


bricks_error_code_e 	
jester_ctx_t::publish(const char* topic, const char*data, size_t size)
{
	auto it = subscribers.find(topic);
	if (it == subscribers.end())
	{
		return BRICKS_SUCCESS;
	}
	
	for (auto s : it->second)
	{
		s->on_received(topic, data, size);
	}

	return BRICKS_SUCCESS;

}

bricks_error_code_e 
jester_ctx_t::subscribe(const char* topic, jester_subscriber_t* s)
{
	auto it = subscribers.find(topic);
	if (it == subscribers.end())
	{
		subscribers[topic] = {};
		it = subscribers.find(topic);
	};

	it->second.push_back(s);

	return BRICKS_SUCCESS;
}

bricks_error_code_e
jester_ctx_t::unsubscribe(jester_subscriber_t* s)
{
	auto it = subscribers.begin();
	while (it != subscribers.end())
	{
		it->second.erase(std::remove(it->second.begin(), it->second.end(),s), it->second.end());

		if (it->second.size() == 0)
			it = subscribers.erase(it);		
		else
			++it;
	}

	return BRICKS_SUCCESS;
}


jester_publisher_t::jester_publisher_t(jester_ctx_t* ctx) :ctx(ctx)
{

}

bricks_error_code_e 
jester_publisher_t::init(cb_queue_t* queue, delivery_cb_t msg_cb, const xtree_t* options)
{
	ASSERT_NOT_INITIATED;

	this->queue = queue;
	this->msg_cb = msg_cb;
	this->initiated = true;

	return BRICKS_SUCCESS;
}

bricks_error_code_e
jester_publisher_t::start()
{
	ASSERT_INITIATED;
	ASSERT_NOT_STARTED;

	started = true;

	return BRICKS_SUCCESS;
}

bricks_error_code_e 
jester_publisher_t::register_topic(const string& topic, const xtree_t* options)
{
	ASSERT_INITIATED;
	ASSERT_NOT_STARTED;

	return BRICKS_SUCCESS;
}

bricks_error_code_e 
jester_publisher_t::publish(const string& topic, const char* data, size_t size, void* opaque, const xtree_t* options)
{
	ASSERT_INITIATED;
	ASSERT_STARTED;

	ctx->publish(topic.c_str(), data, size);

	auto xt = create_xtree();

	queue->enqueue(std::bind(msg_cb, opaque, BRICKS_SUCCESS, xt));

	return BRICKS_SUCCESS;
	
}


jester_subscriber_t::jester_subscriber_t(jester_ctx_t* ctx):ctx(ctx)
{

}

bricks_error_code_e 
jester_subscriber_t::init(cb_queue_t* queue, msg_cb_t msg_cb, const xtree_t* options)
{
	ASSERT_NOT_INITIATED;
	this->queue = queue;
	this->initiated = true;
	this->msg_cb = msg_cb;

	return BRICKS_SUCCESS;
}

bricks_error_code_e 
jester_subscriber_t::register_topic(const string& topic, const xtree_t* options)
{
	ASSERT_INITIATED;
	ASSERT_NOT_STARTED;

	topics.push_back(topic);

	return BRICKS_SUCCESS;
}

bricks_error_code_e
jester_subscriber_t::start()
{
	ASSERT_INITIATED;
	ASSERT_NOT_STARTED;

	for (auto& t : topics)
	{
		ctx->subscribe(t.c_str(), this);
	}

	started = true;

	return BRICKS_SUCCESS;
}

bricks_error_code_e 
jester_subscriber_t::on_received(const char* topic, const char* data, size_t size)
{

	ASSERT_INITIATED;
	ASSERT_STARTED;

	auto xtree = create_xtree();

	queue->enqueue(std::bind(msg_cb,  create_buffer(data, size), xtree));

	return BRICKS_SUCCESS;
}

jester_subscriber_t::~jester_subscriber_t()
{
	ctx->unsubscribe(this);
}





