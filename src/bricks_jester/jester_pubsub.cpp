#include "pch.h"
#include "utils.h"
#include "jester_pubsub.h"

using namespace std;
using namespace bricks;
using namespace bricks::plugins;


brick_t*
bricks::plugins::create_jester_pubsub_context()
{
	return new jester_pubsub_ctx_t();
}

publisher_plugin_t*
bricks::plugins::create_jester_publisher(brick_t* ctx)
{
	return new jester_publisher_t((jester_pubsub_ctx_t*)ctx);
}

BRICKSJESTER_API subscriber_plugin_t*
bricks::plugins::create_jester_subscriber(brick_t* ctx)
{
	return new jester_subscriber_t((jester_pubsub_ctx_t*)ctx);
}



bricks_error_code_e 	
jester_pubsub_ctx_t::publish(const char* topic, const char*data, size_t size)
{
	auto it = subscribers.find(topic);
	if (it == subscribers.end())
	{
		return BRICKS_SUCCESS;
	}
	
	for (auto s : it->second)
	{
		s->topic_cb(topic, data, size);
	}

	return BRICKS_SUCCESS;

}

bricks_error_code_e 
jester_pubsub_ctx_t::subscribe(const char* topic, jester_subscriber_t* s)
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
jester_pubsub_ctx_t::remove_subscription(const char* topic, jester_subscriber_t* s)
{
	auto it = subscribers.begin();
	while (it != subscribers.end())
	{
		if (it->first == topic)
		{
			it->second.erase(std::remove(it->second.begin(), it->second.end(), s), it->second.end());

			if (it->second.size() == 0)
				it = subscribers.erase(it);
			else
				++it;
		}
	}

	return BRICKS_SUCCESS;
}

bricks_error_code_e
jester_pubsub_ctx_t::unsubscribe(jester_subscriber_t* s)
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


jester_publisher_t::jester_publisher_t(jester_pubsub_ctx_t* ctx) :ctx(ctx)
{

}

bricks_error_code_e 
jester_publisher_t::init(cb_queue_t* queue, const xtree_t* options)
{
	SYNCHRONIZED(ctx->mtx);

	ASSERT_NOT_INITIATED;

	this->queue = queue;

	this->initiated = true;

	return BRICKS_SUCCESS;
}

bricks_error_code_e
jester_publisher_t::start()
{
	SYNCHRONIZED(ctx->mtx);

	ASSERT_INITIATED;
	ASSERT_NOT_STARTED;

	started = true;

	return BRICKS_SUCCESS;
}

bricks_error_code_e 
jester_publisher_t::add_topic(const string& topic, const xtree_t* options)
{
	SYNCHRONIZED(ctx->mtx);

	ASSERT_INITIATED;

	return BRICKS_SUCCESS;
}

bricks_error_code_e 
jester_publisher_t::publish(const string& topic, const char* data, size_t size, const xtree_t* options)
{
	SYNCHRONIZED(ctx->mtx);

	ASSERT_INITIATED;
	ASSERT_STARTED;

	ctx->publish(topic.c_str(), data, size);

	return BRICKS_SUCCESS;
	
}

bool
jester_publisher_t::check_capability(plugin_capabilities_e cap)
{
	SYNCHRONIZED(ctx->mtx);

	switch (cap)
	{
	case HIERARCHICAL_SUBSCRIBE:
		return true;
	default:
		return false;
	}
}

jester_subscriber_t::jester_subscriber_t(jester_pubsub_ctx_t* ctx):ctx(ctx)
{

}

bricks_error_code_e 
jester_subscriber_t::init(cb_queue_t* queue, topic_cb_t msg_cb, const xtree_t* options)
{
	SYNCHRONIZED(ctx->mtx);

	ASSERT_NOT_INITIATED;

	this->queue = queue;
	this->msg_cb = msg_cb;

	this->initiated = true;

	return BRICKS_SUCCESS;
}

bricks_error_code_e 
jester_subscriber_t::unsubscribe(const string& topic)
{
	SYNCHRONIZED(ctx->mtx);

	ASSERT_INITIATED;

	auto new_end = std::remove(topics.begin(), topics.end(), topic);
	topics.erase(new_end, topics.end());

	ctx->remove_subscription(topic.c_str(), this);

	return BRICKS_SUCCESS;
}

bricks_error_code_e jester_subscriber_t::unsubscribe()
{
	SYNCHRONIZED(ctx->mtx);

	ASSERT_INITIATED;

	topics.clear();
	ctx->unsubscribe(this);

	return BRICKS_SUCCESS;
}



bricks_error_code_e 
jester_subscriber_t::subscribe(const string& topic, const xtree_t* options)
{
	SYNCHRONIZED(ctx->mtx);

	ASSERT_INITIATED;
	
	topics.push_back(topic);

	return BRICKS_SUCCESS;
}

bricks_error_code_e
jester_subscriber_t::start()
{
	SYNCHRONIZED(ctx->mtx);

	ASSERT_INITIATED;
	ASSERT_NOT_STARTED;

	for (auto& t : topics)
	{
		ctx->subscribe(t.c_str(), this);
	}

	this->started = true;

	return BRICKS_SUCCESS;
}

bricks_error_code_e 
jester_subscriber_t::topic_cb(const char* topic, const char* data, size_t size)
{
	SYNCHRONIZED(ctx->mtx);

	ASSERT_INITIATED;
	ASSERT_STARTED;

	auto xtree = create_xtree();

	queue->enqueue(std::bind(msg_cb,topic,create_buffer(data, size), xtree));

	return BRICKS_SUCCESS;
}

jester_subscriber_t::~jester_subscriber_t()
{
	SYNCHRONIZED(ctx->mtx);

	ctx->unsubscribe(this);
}

bool 
jester_subscriber_t::check_capability(plugin_capabilities_e cap)
{
	SYNCHRONIZED(ctx->mtx);

	switch (cap)
	{
	case HIERARCHICAL_SUBSCRIBE:
		return true;
	default:
		return false;
	}
}



