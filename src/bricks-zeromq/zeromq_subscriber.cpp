#include "pch.h"
#include "zeromq_subscriber.h"

using namespace bricks;
using namespace bricks::plugins;


zeromq_subscriber_t::zeromq_subscriber_t()
{

};

void
zeromq_subscriber_t::destroy()
{
	initiated = false;
	started = false;

	stop_zmq_poll_loop();

	if (subscriber)
	{
		zmq_close(subscriber);
		subscriber = nullptr;
	}

	if (context)
	{
		zmq_ctx_destroy(context);
		context = nullptr;
	}
}

bricks_error_code_e
zeromq_subscriber_t::init(cb_queue_t* queue, msg_cb_t msg_cb, const xtree_t* options)
{
	ASSERT_NOT_INITIATED;

	bricks_error_code_e err = BRICKS_SUCCESS;

	context = zmq_ctx_new();

	subscriber = zmq_socket(context, ZMQ_SUB);

	try
	{
		url = get<string>(options->get_property_value("/configuration/subscriber", "url").value());

		// Connect to the ZeroMQ endpoint
		auto rc = zmq_connect(subscriber, url.c_str());
		if (rc != 0) {
			throw std::exception();
		}

		this->msg_cb = msg_cb;

		initiated = true;
		
	}
	catch (std::bad_optional_access&)
	{
		err = BRICKS_INVALID_PARAM;
	}
	catch (std::exception&)
	{
		err = BRICKS_FAILURE_GENERIC;
	}

	if (err != BRICKS_SUCCESS)
	{
		destroy();
	}

	return err;

}

zeromq_subscriber_t::~zeromq_subscriber_t()
{
	destroy();
}

bricks_error_code_e
zeromq_subscriber_t::register_topic(const string& topic, const xtree_t* options)
{
	ASSERT_INITIATED;
	ASSERT_NOT_STARTED;

	auto rc = zmq_setsockopt(subscriber, ZMQ_SUBSCRIBE, topic.c_str(), 0);
	if (rc != 0)
	{
		return  BRICKS_3RD_PARTY_ERROR;
	}

	// Create a ZMQ poll item
	items[0] = { subscriber, 0, ZMQ_POLLIN, 0 };

	return BRICKS_SUCCESS;

}

bricks_error_code_e 
zeromq_subscriber_t::start()
{
	ASSERT_INITIATED;
	ASSERT_NOT_STARTED;

	auto rc = start_zmq_poll_loop();
	if (rc == BRICKS_SUCCESS)
	{
		started = true;
	}

	return rc;

}

bricks_error_code_e
zeromq_subscriber_t::do_zmq_poll(int milliseconds, bool last_call)
{

	ASSERT_INITIATED;
	ASSERT_STARTED;

	// Poll for incoming data
	zmq_poll(items, 1, (long)milliseconds);

	// If there's data available, receive it
	if (items[0].revents & ZMQ_POLLIN) {
		
		zmq_msg_t msg;
		zmq_msg_init(&msg);
		zmq_msg_recv(&msg, subscriber, 0);

		auto xt = create_xtree();

		cb_queue->enqueue(
			std::bind(msg_cb, create_buffer((const char*)zmq_msg_data(&msg), (int)zmq_msg_size(&msg)), xt)
		);

	}

	return BRICKS_NOT_SUPPORTED;
}

