#include "pch.h"
#include "redispp_subscriber.h"
#include "redispp_plugin.h"

using namespace std;
using namespace bricks;
using namespace bricks::plugins;
using namespace std::placeholders;



subscriber_plugin_t*
bricks::plugins::create_redispp_subscriber()
{
	return new redispp_subscriber_t();
}

redispp_subscriber_t::redispp_subscriber_t()
{

}

redispp_subscriber_t::~redispp_subscriber_t()
{
	SYNCHRONIZED(mtx);

	destroy();
}

void
redispp_subscriber_t::destroy()
{
	if (destroyed)
		return;

	destroyed = true;

	if (subscriber)
	{
		try {
			auto f = subscriber->unsubscribe();
			f.wait();
		}
		catch (std::exception&) {};
		delete subscriber;
		subscriber = nullptr;
	}

	if (redis)
	{
		delete redis;
		redis = nullptr;
	}
}

bricks_error_code_e
redispp_subscriber_t::init(cb_queue_t* queue, topic_cb_t msg_cb, const xtree_t* options)
{

	SYNCHRONIZED(mtx);

	ASSERT_PREINIT;

	this->queue = queue;

	this->msg_cb = msg_cb;
	
	bricks_error_code_e err = BRICKS_SUCCESS;

	try
	{
		name = get<string>(options->get_property_value("/bricks/redispp/subscriber", "name").value());

		url = get<string>(options->get_property_value("/bricks/redispp/subscriber/methods/init/connection", "url").value());

		redis = new AsyncRedis(url.c_str());

		subscriber = new AsyncSubscriber(redis->subscriber());

		subscriber->on_message(std::bind(&redispp_subscriber_t::on_topic, this, _1, _2));

		subscriber->on_meta(std::bind(&redispp_subscriber_t::on_meta, this, _1, _2, _3));

		subscriber->on_error(std::bind(&redispp_subscriber_t::on_error, this, _1));

		initiated = true;
	}
	catch (Error& e)
	{
		err = BRICKS_3RD_PARTY_ERROR;
		log1(BRICKS_ALARM, "%s %%%%%% Failed to create redis interface: '%s'.\n", this->name.c_str(), e.what());
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



bool 
redispp_subscriber_t::check_capability(plugin_capabilities_e e)
{
	SYNCHRONIZED(mtx);

	return false;
}


void 
redispp_subscriber_t::on_meta(Subscriber::MsgType type, OptionalString channel, long long num)
{
	log1(BRICKS_DEBUG, "%s %%%%%% Event %d on topic (where relevant) '%s' (%d).\n", this->name.c_str(), type, channel ? channel.value().c_str():"", num);
}

void 
redispp_subscriber_t::on_error(std::exception_ptr eptr)
{
	try
	{
		if (eptr)
			std::rethrow_exception(eptr);
	}
	catch (const std::exception& e)
	{
		log1(BRICKS_DEBUG, "%s %%%%%% Exception %s \n", this->name.c_str(), e.what());
	}
	
}

void 
redispp_subscriber_t::on_topic(std::string channel, std::string msg)
{
	SYNCHRONIZED(mtx);

	string opt = "";

	auto xt = create_xtree_from_xml((opt +
		"<bricks>" +
		"  <event last=\"true\"/>" +
		"</bricks>").c_str()
	);


	this->queue->enqueue(
		std::bind(
			msg_cb,
			channel,
			create_buffer((const char*)msg.data(), (int)msg.size()),
			xt)
	);
}

bricks_error_code_e 
redispp_subscriber_t::subscribe(const string& topic, const xtree_t* options)
{
	SYNCHRONIZED(mtx);

	ASSERT_READY;

	bricks_error_code_e err = BRICKS_SUCCESS;

	try
	{
		subscriber->subscribe(topic);
	}
	catch (Error& e)
	{
		err = BRICKS_3RD_PARTY_ERROR;
		log1(BRICKS_ALARM, "%s %%%%%% Failed to subscribe to topic %s : '%s'.\n", this->name.c_str(), topic.c_str(), e.what());
	}

	return err;

}

bricks_error_code_e 
redispp_subscriber_t::unsubscribe(const string& topic, const xtree_t* options)
{
	SYNCHRONIZED(mtx);

	ASSERT_READY;

	bricks_error_code_e err = BRICKS_SUCCESS;

	try
	{
		if (topic == "")
			subscriber->unsubscribe();
		else
			subscriber->unsubscribe(topic);
	}
	catch (Error& e)
	{
		err = BRICKS_3RD_PARTY_ERROR;
		log1(BRICKS_ALARM, "%s %%%%%% Failed to unsubscribe from topic %s: '%s'.\n", this->name.c_str(), topic.c_str(), e.what());
	}

	return err;
}


