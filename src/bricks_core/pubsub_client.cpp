#include "pch.h"
#include "bricks.h"
#include "pubsub_client.h"
#include "meta.h"

using namespace bricks;
using namespace std;
using namespace std::placeholders;

client_plugin_t* bricks::create_pubsub_client(
	publisher_plugin_t* publisher,
	subscriber_plugin_t* subscriber,
	timer_t* timer,
	const char* request_topic_prefix,
	const char* response_topic_prefix,
	const char* error_topic_prefix)
{
	return new pubsub_client_t(publisher, subscriber, timer, request_topic_prefix, response_topic_prefix, error_topic_prefix);
}

std::random_device rd; // Obtain a random number from hardware
std::mt19937_64 gen(rd()); // Seed the generator (Mersenne Twister for 64-bit numbers)
std::uniform_int_distribution<uint64_t> distrib(1, UINT64_MAX); // Define the rang

pubsub_client_t::pubsub_client_t(
	publisher_plugin_t* publisher,
	subscriber_plugin_t* subscriber,
	timer_t* timer,
	const char* request_topic_prefix,
	const char* response_topic_prefix,
	const char* error_topic_prefix) :
	publisher(publisher),
	subscriber(subscriber), 
	timer(timer),
	request_topic_prefix(request_topic_prefix),
	response_topic_prefix(response_topic_prefix),
	error_topic_prefix(error_topic_prefix)
{

}

bool 
pubsub_client_t::check_capability(plugin_capabilities_e)
{
	SYNCHRONIZED(mtx);

	return false;
}

bricks_error_code_e 
pubsub_client_t::init(cb_queue_t* queue, int timeout_ms, const xtree_t* options)
{
	SYNCHRONIZED(mtx);

	this->queue = queue;

	bricks_error_code_e err = BRICKS_SUCCESS;

	if ((err = publisher->init(queue, options)) != BRICKS_SUCCESS)
	{
		return err;
	};

	if ((err = subscriber->init(queue, std::bind(&pubsub_client_t::topic_cb, this,_1,_2,_3),options)) != BRICKS_SUCCESS)
	{
		return err;
	}

	return BRICKS_SUCCESS;
}

bricks_error_code_e 
pubsub_client_t::issue_request(const char* buf, size_t size, response_cb_t client_cb, const xtree_t* options)
{
	SYNCHRONIZED(mtx);

	int64_t req_handle = distrib(gen);

	char hex_buffer[64];

	snprintf(hex_buffer, 64, "0x%" PRIx64, req_handle);

	string request_topic	= request_topic_prefix + "/"  + hex_buffer;
	string response_topic	= response_topic_prefix + "/" + hex_buffer;
	string error_topic		= error_topic_prefix + "/" + hex_buffer;

	bricks_error_code_e err = BRICKS_SUCCESS;

	do {

		if ((err = subscriber->subscribe(response_topic, options)) != BRICKS_SUCCESS)
			break;

		if ((err = subscriber->subscribe(error_topic, options)) != BRICKS_SUCCESS)
			break;

		if ((err = publisher->describe_topic(response_topic, options)) != BRICKS_SUCCESS)
			break;

		if ((err = publisher->publish(request_topic, buf,size, options)) != BRICKS_SUCCESS)
			break;

		int handle = 0;
		if ((err = timer->schedule_timer(
				std::bind(&pubsub_client_t::timeout_cb, this, response_topic), 
				std::chrono::milliseconds(timeout_ms), 
				handle)) != BRICKS_SUCCESS)
			break;
	
		ctxs[response_topic] = { client_cb, handle, request_topic, response_topic,error_topic };
		
	} while(false);

	if (err != BRICKS_SUCCESS)
	{
		subscriber->unsubscribe(response_topic);
		subscriber->unsubscribe(error_topic);
	}

	return BRICKS_SUCCESS;
}


void pubsub_client_t::timeout_cb(const string& topic)
{
	SYNCHRONIZED(mtx);

	auto iter = ctxs.find(topic);
	if (iter == ctxs.end())
	{
		return;
	}

	subscriber->unsubscribe(iter->second.response_topic);
	subscriber->unsubscribe(iter->second.error_topic);

	queue->enqueue(std::bind(iter->second.request_cb, BRICKS_TIMEOUT, nullptr, nullptr));
	
	ctxs.erase(iter);
}

void 
pubsub_client_t::topic_cb(const string& topic, buffer_t *buf, xtree_t* xt)
{
	SYNCHRONIZED(mtx);

	auto iter = ctxs.find(topic);
	if (iter == ctxs.end())
	{
		return;
	}

	subscriber->unsubscribe(iter->second.response_topic);
	subscriber->unsubscribe(iter->second.error_topic);


	queue->enqueue(std::bind(iter->second.request_cb, BRICKS_SUCCESS, buf, xt));

	timer->cancel_timer(iter->second.handle);

	ctxs.erase(iter);

}