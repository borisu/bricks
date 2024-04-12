#include "pch.h"
#include "zeromq_publisher.h"

using namespace bricks;
using namespace bricks::plugins;

zeromq_publisher_t::zeromq_publisher_t()
{


};


bricks_error_code_e 
zeromq_publisher_t::init(cb_queue_t* queue, delivery_cb_t msg_cb, const xtree_t* options)
{
	ASSERT_NOT_INITIATED;

	bricks_error_code_e err = BRICKS_SUCCESS;

	cb_queue = queue;

	context = zmq_ctx_new();

	publisher = zmq_socket(context, ZMQ_PUB);

	try
	{
		auto url = get<string>(options->get_property_value("/configuration/publisher", "url").value());

		int rc = zmq_bind(publisher, url.c_str());
		if (rc != 0) {
			throw std::exception();
		}
	}
	catch (std::bad_optional_access&)
	{
		err = BRICKS_INVALID_PARAM;
	}
	catch (std::exception&)
	{
		err = BRICKS_3RD_PARTY_ERROR;
	}

	this->msg_cb = msg_cb;
	
	if (err != BRICKS_SUCCESS)
	{
		destroy();
	}

	return err;


}

zeromq_publisher_t::~zeromq_publisher_t()
{
	destroy();
}

void
zeromq_publisher_t::destroy()
{
	if (publisher)
	{
		zmq_close(publisher);
		publisher = nullptr;
	}

	if (context)
	{
		zmq_ctx_destroy(context);
		context = nullptr;
	}
}

bricks_error_code_e 
zeromq_publisher_t::register_topic(const string& topic, const xtree_t* options)
{
	ASSERT_INITIATED;
	ASSERT_NOT_STARTED;

	return BRICKS_SUCCESS;

}

bricks_error_code_e 
zeromq_publisher_t::publish(const string& topic, const char* buf , size_t size, void* opaque, const xtree_t* options)
{
	ASSERT_INITIATED;
	ASSERT_STARTED;

	auto rc = zmq_send(publisher, buf, size, 0);

	auto xt = create_xtree();

	if (msg_cb)
	{
		cb_queue->enqueue(std::bind(msg_cb, opaque, rc > 0 ? BRICKS_SUCCESS : BRICKS_3RD_PARTY_ERROR, xt));
	}

	return BRICKS_SUCCESS;

}

bricks_error_code_e zeromq_publisher_t::start()
{
	ASSERT_INITIATED;
	ASSERT_NOT_STARTED;

	started = true;

	return BRICKS_SUCCESS;
}


