#include "pch.h"
#include "rabbitmq_publisher.h"
#include "rabbitmq_plugin.h"
#include "rabbitmq_utils.h"

using namespace std;
using namespace bricks;
using namespace bricks::plugins;



publisher_plugin_t*
bricks::plugins::create_rabbitmq_publisher()
{
	return new rabbitmq_publisher_t();
}

rabbitmq_publisher_t::rabbitmq_publisher_t()
{

}

rabbitmq_publisher_t::~rabbitmq_publisher_t()
{
	SYNCHRONIZED(mtx);

	destroy();
}

void
rabbitmq_publisher_t::destroy()
{
	SYNCHRONIZED(mtx);

	rabbitmq_base_t::destroy();

}

bool 
rabbitmq_publisher_t::check_capability(plugin_capabilities_e e)
{
	SYNCHRONIZED(mtx);

	return false;
}

bricks_error_code_e
rabbitmq_publisher_t::init(cb_queue_t* queue, const xtree_t* options)
{

	SYNCHRONIZED(mtx);

	ASSERT_NOT_INITIATED;

	name = get_opt<string>(options->get_property_value("/bricks/rabbitmq/publisher", "name"));

	auto err = rabbitmq_base_t::handle_connect_options("/bricks/rabbitmq/publisher/init/connection", queue, options);

	if (err == BRICKS_SUCCESS)
	{
		initiated = true;
	}

	return err;
}


bricks_error_code_e 
rabbitmq_publisher_t::describe_topic(const string& topic, const xtree_t* options)
{
	SYNCHRONIZED(mtx);

	ASSERT_INITIATED;
	ASSERT_NOT_STARTED;

	return handle_exchange_declare_options("/bricks/rabbitmq/publisher/describe_topic/exchange", topic, options);

}




bricks_error_code_e 
rabbitmq_publisher_t::start()
{
	SYNCHRONIZED(mtx);

	ASSERT_INITIATED;
	ASSERT_NOT_STARTED;

	started = true;

	return BRICKS_SUCCESS;

}

bricks_error_code_e 
rabbitmq_publisher_t::publish(const string& topic, const char* data, size_t size, const xtree_t* options)
{
	SYNCHRONIZED(mtx);

	ASSERT_INITIATED;
	ASSERT_STARTED;
	
	return handle_publish_options("/bricks/rabbitmq/publisher/publish", topic, data, size, options);

}