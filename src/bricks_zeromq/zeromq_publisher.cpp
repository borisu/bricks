#include "pch.h"
#include "zeromq_publisher.h"

using namespace bricks;
using namespace bricks::plugins;

zeromq_publisher_t::zeromq_publisher_t()
{


};

bool 
zeromq_publisher_t::check_capability(plugin_capabilities_e e)
{
	switch (e)
	{
	case HIERARCHICAL_SUBSCRIBE:
		return true;
	default:
		return false;
	}

}

bricks_error_code_e 
zeromq_publisher_t::init(cb_queue_t* queue, const xtree_t* options)
{
	ASSERT_NOT_INITIATED;

	bricks_error_code_e err = BRICKS_SUCCESS;

	cb_queue = queue;

	context = zmq_ctx_new();

	publisher = zmq_socket(context, ZMQ_PUB);

	try
	{
		set_sockopt(options, "/bricks/zmq/publisher", publisher);

		name = get<string>(options->get_property_value("/bricks/zmq/publisher", "name").value());

		auto url = get<string>(options->get_property_value("/bricks/zmq/publisher", "url").value());

		auto is_server = get<bool>(options->get_property_value("/bricks/zmq/publisher", "is_server").value());

		int rc =  is_server  ? zmq_bind(publisher, url.c_str()) : zmq_connect(publisher, url.c_str());

		if (rc != 0) {
			is_server ? BRICK_LOG_ZMQ_ERROR(zmq_bind) : BRICK_LOG_ZMQ_ERROR(zmq_connect);
			throw std::exception();
		}

		initiated = true;
	}
	catch (std::bad_optional_access&)
	{
		err = BRICKS_INVALID_PARAM;
	}
	catch (std::exception&)
	{
		err = BRICKS_3RD_PARTY_ERROR;
	}

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
zeromq_publisher_t::add_topic(const string& topic, const xtree_t* options)
{
	ASSERT_INITIATED;
	ASSERT_NOT_STARTED;

	return BRICKS_SUCCESS;

}

bricks_error_code_e 
zeromq_publisher_t::publish(const string& topic, const char* buf , size_t size,  const xtree_t* options)
{
	ASSERT_INITIATED;
	ASSERT_STARTED;

	auto rc = zmq_send(publisher, topic.c_str(), strlen(topic.c_str()), ZMQ_SNDMORE);
	if (rc == -1) {
		BRICK_LOG_ZMQ_ERROR(zmq_send);
		throw std::exception();
	}

	rc = zmq_send(publisher, buf, size, 0);
	if (rc == -1) {
		BRICK_LOG_ZMQ_ERROR(zmq_send);
		throw std::exception();
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


