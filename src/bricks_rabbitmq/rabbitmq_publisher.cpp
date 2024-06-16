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

	if (meta_cb)
		this->queue->enqueue(std::bind(meta_cb, PLUGIN_DESTROYED, nullptr));

}

void
rabbitmq_publisher_t::set_meta_cb(meta_cb_t meta_cb)
{
	SYNCHRONIZED(mtx);

	this->meta_cb = meta_cb;
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

	ASSERT_PREINIT;

	name = get_opt<string>(options->get_property_value("/bricks/rabbitmq/publisher", "name"));

	auto err = rabbitmq_base_t::handle_connect_options("/bricks/rabbitmq/publisher/methods/init/connection", queue, options);

	if (err == BRICKS_SUCCESS)
	{
		initiated = true;
	}
	else
	{
		destroy();
	}

	return err;
}

bricks_error_code_e 
rabbitmq_publisher_t::describe_topic(const string& topic, const xtree_t* options)
{
	SYNCHRONIZED(mtx);

	ASSERT_READY;

	return handle_exchange_declare_options("/bricks/rabbitmq/publisher/methods/describe_topic/exchange", topic, options);
}

bricks_error_code_e 
rabbitmq_publisher_t::publish(const string& topic, const char* data, size_t size, const xtree_t* options)
{
	SYNCHRONIZED(mtx);

	ASSERT_READY;
	
	return handle_publish_options("/bricks/rabbitmq/publisher/methods/publish", topic, data, size, options);
}