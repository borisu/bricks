#include "pch.h"
#include "redispp_publisher.h"
#include "redispp_plugin.h"

using namespace std;
using namespace bricks;
using namespace bricks::plugins;



publisher_plugin_t*
bricks::plugins::create_redispp_publisher()
{
	return new redispp_publisher_t();
}

redispp_publisher_t::redispp_publisher_t()
{

}

redispp_publisher_t::~redispp_publisher_t()
{
	SYNCHRONIZED(mtx);

	destroy();
}

void
redispp_publisher_t::destroy()
{
	SYNCHRONIZED(mtx);

	initiated = false;

	started = false;

	if (redis)
	{
		delete redis;
		redis = nullptr;
	}
}

bool 
redispp_publisher_t::check_capability(plugin_capabilities_e e)
{
	SYNCHRONIZED(mtx);

	switch (e)
	{
	case HIERARCHICAL_PUBLISH:
		return true;
	default:
		return false;
	}
}


bricks_error_code_e 
redispp_publisher_t::add_topic(const string& topic, const xtree_t* options)
{
	SYNCHRONIZED(mtx);

	return BRICKS_SUCCESS;
}

bricks_error_code_e 
redispp_publisher_t::start()
{
	SYNCHRONIZED(mtx);

	ASSERT_INITIATED;
	ASSERT_NOT_STARTED;

	started = true;

	return BRICKS_SUCCESS;

}
bricks_error_code_e
redispp_publisher_t::init(cb_queue_t* queue, const xtree_t* options)
{

	SYNCHRONIZED(mtx);

	ASSERT_NOT_INITIATED;

	this->queue = queue;

	bricks_error_code_e err = BRICKS_SUCCESS;

	try
	{
		name = get<string>(options->get_property_value("/bricks/redispp/publisher", "name").value());

		url = get<string>(options->get_property_value("/bricks/redispp/publisher", "url").value());

		redis = new AsyncRedis(url.c_str());

		initiated = true;
	}
	catch (Error& e)
	{
		err = BRICKS_3RD_PARTY_ERROR;
		log1(BRICKS_ALARM, "%s %%%%%% Failed to create redis++ interface from url %s : '%s'.\n", this->name.c_str(), url.c_str(), e.what());
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

bricks_error_code_e 
redispp_publisher_t::publish(const string& topic, const char* data, size_t size, const xtree_t* options)
{
	SYNCHRONIZED(mtx);

	ASSERT_INITIATED;

	ASSERT_STARTED;

	bricks_error_code_e err = BRICKS_SUCCESS;

	try
	{

	auto fut = redis->publish(topic.c_str(), StringView(data, size));
	fut.wait();

	}
	catch (Error& e)
	{
		err = BRICKS_3RD_PARTY_ERROR;
		log1(BRICKS_ALARM, "%s %%%%%% Failed to publish to topic %s : '%s'.\n", this->name.c_str(), topic.c_str(), e.what());
	}

	return err;

}