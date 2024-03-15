#include "pch.h"
#include "zeromq_publisher.h"

zeromq_publisher_t::zeromq_publisher_t()
{


};

bricks_error_code_e 
zeromq_publisher_t::init(delivery_cb_t msg_cb, const xtree_t* options)
{
	ZMQ_ASSERT_NOT_INITIATED;

	bricks_error_code_e err = BRICKS_SUCCESS;

	context = zmq_ctx_new();

	publisher = zmq_socket(context, ZMQ_PUB);

	try
	{
		auto url = options->get_property_value_as_string("/configuration/publisher", "url").value();

		int rc = zmq_bind(publisher, url.c_str());
		if (rc != 0)
		{
			err =  BRICKS_3RD_PARTY_ERROR;
		}

	}
	catch (std::bad_optional_access&)
	{
		err = BRICKS_INVALID_PARAM;
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

	return BRICKS_SUCCESS;

}

bricks_error_code_e 
zeromq_publisher_t::publish(const string& topic, const buffer_t& buf, void* opaque, const xtree_t* options)
{
	auto rc = zmq_send(publisher, &buf[0], buf.size(), 0);

	auto xt = create_xtree();

	if (msg_cb)
		msg_cb(opaque, rc > 0 ? BRICKS_SUCCESS : BRICKS_3RD_PARTY_ERROR, *xt);

	destroy_xtree(xt);

	return rc > 0 ? BRICKS_SUCCESS : BRICKS_3RD_PARTY_ERROR;

}

bricks_error_code_e 
zeromq_publisher_t::poll(size_t timeout)
{
	return BRICKS_NOT_SUPPORTED;
}

