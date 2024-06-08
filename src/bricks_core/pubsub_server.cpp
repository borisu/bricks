#include "pch.h"
#include "plugin.h"
#include "pubsub_server.h"
#include "utils.h"
#include "meta.h"

using namespace bricks;
using namespace std;
using namespace std::placeholders;

server_plugin_t* bricks::create_pubsub_server(
	publisher_plugin_t* publisher,
	subscriber_plugin_t* subscriber,
	const char* request_topic_prefix,
	const char* response_topic_prefix,
	const char* error_topic_prefix)
{
    if (!subscriber->check_capability(HIERARCHICAL_SUBSCRIBE))
		return nullptr;

	return new pubsusb_server_t(publisher, subscriber, request_topic_prefix, response_topic_prefix, error_topic_prefix);
}

pubsusb_server_t::pubsusb_server_t(
	publisher_plugin_t* publisher,
	subscriber_plugin_t* subscriber,
	const char* request_topic_prefix,
	const char* response_topic_prefix,
	const char* error_topic_prefix) :
	publisher(publisher),
	subscriber(subscriber),
	request_topic_prefix(request_topic_prefix),
	response_topic_prefix(response_topic_prefix),
	error_topic_prefix(error_topic_prefix)
{

}

bricks_error_code_e
pubsusb_server_t::init(cb_queue_t* queue,  const xtree_t* options)
{
	SYNCHRONIZED(mtx);

	ASSERT_NOT_INITIATED;
	ASSERT_NOT_STARTED;

	this->queue = queue;

	bricks_error_code_e err = BRICKS_SUCCESS;

	if ((err = publisher->init(queue, options)) != BRICKS_SUCCESS)
	{
		return err;
	};

	if ((err = subscriber->init(queue, std::bind(&pubsusb_server_t::topic_cb, this, _1, _2, _3), options)) != BRICKS_SUCCESS)
	{
		return err;
	}

	if ((err = subscriber->subscribe(request_topic_prefix, options)) != BRICKS_SUCCESS)
	{
		return err;
	}

	initiated = true;

	return BRICKS_SUCCESS;
}

bricks_error_code_e
pubsusb_server_t::register_request_cb(request_cb_t request_cb, const xtree_t* options)
{
	SYNCHRONIZED(mtx);

	ASSERT_INITIATED;
	ASSERT_NOT_STARTED;
	
	this->request_cb = request_cb;

	return BRICKS_SUCCESS;
}

bricks_error_code_e
pubsusb_server_t::start()
{
	SYNCHRONIZED(mtx);

	ASSERT_INITIATED;
	ASSERT_NOT_STARTED;

	bricks_error_code_e err = BRICKS_SUCCESS;

	if ((err = subscriber->start()) != BRICKS_SUCCESS)
		return err;

	if ((err = publisher->start()) != BRICKS_SUCCESS)
	{
		return err;
	}

	started = true;

	return BRICKS_SUCCESS;
}


void
pubsusb_server_t::topic_cb(const string& topic, buffer_t* buf, xtree_t* xt)
{
	SYNCHRONIZED(mtx);

	size_t delimiter = topic.find_last_of("/");
	if (delimiter == string::npos || delimiter == (topic.length()-1))
		return;

	string response_postfix = response_topic_prefix + "/" + topic.substr(delimiter);
	
	response_proxy_cb_t p = std::bind(&pubsusb_server_t::response_proxy_cb, this, response_postfix, _1, _2, _3, _4);

	callback_t cb = std::bind(
		request_cb,
		p,
		buf,
		xt);

	queue->enqueue(cb);

	return;

}

void 
pubsusb_server_t::response_proxy_cb(const string& topic, bricks_error_code_e err, const char* data, size_t size, xtree_t* options)
{
	SYNCHRONIZED(mtx);

	if (err == BRICKS_SUCCESS)
		publisher->publish(topic,data, size, options);
	
}

bool 
pubsusb_server_t::check_capability(plugin_capabilities_e)
{
	SYNCHRONIZED(mtx);

	return false;
}
