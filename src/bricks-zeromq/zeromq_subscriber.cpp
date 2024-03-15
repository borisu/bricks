#include "pch.h"
#include "zeromq_subscriber.h"


zeromq_subscriber_t::zeromq_subscriber_t()
{


};

void
zeromq_subscriber_t::destroy()
{
	initiated = false;
	started = false;

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
zeromq_subscriber_t::init(msg_cb_t msg_cb, const xtree_t* options)
{
	ZMQ_ASSERT_NOT_INITIATED;

	bricks_error_code_e err = BRICKS_SUCCESS;

	context = zmq_ctx_new();

	subscriber = zmq_socket(context, ZMQ_SUB);

	try
	{
		do {
			url = options->get_property_value_as_string("/configuration/subscriber", "url").value();
		} while (false);

		this->msg_cb = msg_cb;

		initiated = true;
		
	}
	catch (std::bad_optional_access&)
	{
		err = BRICKS_INVALID_PARAM;
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
	ZMQ_ASSERT_INITIATED;
	ZMQ_ASSERT_NOT_STARTED;

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
zeromq_subscriber_t::subscribe(void* opaque, const xtree_t* options)
{
	ZMQ_ASSERT_INITIATED;
	ZMQ_ASSERT_NOT_STARTED;

	this->opaque = opaque;

	// Connect to the ZeroMQ endpoint
	auto rc = zmq_connect(subscriber, url.c_str());
	if (rc != 0)
	{
		return BRICKS_3RD_PARTY_ERROR;
	}

	started = true;

	return BRICKS_SUCCESS;

}

bricks_error_code_e
zeromq_subscriber_t::poll(size_t timeout)
{
	ZMQ_ASSERT_INITIATED;
	ZMQ_ASSERT_STARTED;

	// Poll for incoming data
	zmq_poll(items, 1, (long)timeout);

	// If there's data available, receive it
	if (items[0].revents & ZMQ_POLLIN) {
		auto xt = create_xtree();
		zmq_msg_t msg;
		zmq_msg_init(&msg);
		zmq_msg_recv(&msg, subscriber, 0);

		msg_cb(opaque, (const char*)zmq_msg_data(&msg), zmq_msg_size(&msg), *xt);

		destroy_xtree(xt);
	
	}

	return BRICKS_NOT_SUPPORTED;
}




