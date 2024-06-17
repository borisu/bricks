#include "pch.h"
#include "gtest/gtest.h"
#include "generic_tests.h"
#include "utils.h"

using namespace bricks;

#define TEST_TOPIC "bricks.test.17"

bricks_error_code_e 
publish(publisher_plugin_t* publisher, const char *topic, xtree_t* publish_xt, list<string> &cache,  int count, int &offset)
{
	for (int i = 0; i < count; i++, offset++)
	{
		string msg = "Another brick in the wall ";
		msg += std::to_string(i);
		cache.push_back(msg);

		auto res = publisher->publish(topic, msg.c_str(), msg.length(), publish_xt);
		if (res != BRICKS_SUCCESS)
			return res;
	}

	return BRICKS_SUCCESS;
}

void
publish_subscribe_test_2(
	publisher_plugin_t* publisher,
	subscriber_plugin_t* subscriber,
	selector_t* selector,
	xtree_t* xt)
{
	list<string>   all_snd_messages;
	list<msg_info*> all_rcv_messages;

	{
		int offset = 0;

		brick_uptr<brick_t> c(create_poller(BRICKS_DEFAULT_CLIENT_TIMEOUT, selector));


		auto on_topic_cb = [&](const string& topic, buffer_t* buf, xtree_t* xt)
			{
				all_rcv_messages.push_back(new msg_info{ topic, buf, xt });
			};

		/*
		* Prepare publisher
		*/

		ASSERT_EQ(BRICKS_SUCCESS, publisher->init(selector->queue(), xt));
		ASSERT_EQ(BRICKS_SUCCESS, publisher->describe_topic(TEST_TOPIC, xt));

		/*
		* Prepare subscriber.
		*/
		ASSERT_EQ(BRICKS_SUCCESS, subscriber->init(selector->queue(), on_topic_cb, xt));
		ASSERT_EQ(BRICKS_SUCCESS, subscriber->subscribe(TEST_TOPIC, xt));

		this_thread::sleep_for(chrono::milliseconds(STABILIZATION_TIMEOUT));

		publish(publisher, TEST_TOPIC, xt, all_snd_messages, NUM_OF_ITERATIONS, offset);

		this_thread::sleep_for(chrono::milliseconds(STABILIZATION_TIMEOUT));

		ASSERT_EQ(BRICKS_SUCCESS, subscriber->unsubscribe(TEST_TOPIC));

		this_thread::sleep_for(chrono::milliseconds(STABILIZATION_TIMEOUT));

		publish(publisher, TEST_TOPIC, xt, all_snd_messages, NUM_OF_ITERATIONS, offset);

		this_thread::sleep_for(chrono::milliseconds(STABILIZATION_TIMEOUT));

	}

	ASSERT_EQ(all_rcv_messages.size(), NUM_OF_ITERATIONS);

	for (auto i : all_rcv_messages)
	{
		delete i;
	}
}

void
publish_subscribe_test_1(
	xtree_t *publisher_xt, 
	publisher_plugin_t* publisher, 
	xtree_t* subscriber_xt, 
	subscriber_plugin_t* subscriber, 
	selector_t *selector,
	xtree_t* publisher_describe_topic_xt,
	xtree_t* subscriber_decribe_topic_xt,
	xtree_t* publish_xt,
	xtree_t* subscribe_xt)
{

	list<string>   all_snd_messages;
	list<msg_info*> all_rcv_messages;

	{
		int offset = 0;

		brick_uptr<brick_t> c(create_poller(BRICKS_DEFAULT_CLIENT_TIMEOUT, selector));

		
		auto on_topic_cb = [&](const string& topic, buffer_t* buf, xtree_t* xt)
			{
				all_rcv_messages.push_back(new msg_info{ topic, buf, xt });
			};

		/*
		* Prepare publisher
		*/

		ASSERT_EQ(BRICKS_SUCCESS, publisher->init(selector->queue(), publisher_xt));
		ASSERT_EQ(BRICKS_SUCCESS, publisher->describe_topic(TEST_TOPIC, publisher_describe_topic_xt));
		

		/*
		* Prepare subscriber.
		*/
		ASSERT_EQ(BRICKS_SUCCESS, subscriber->init(selector->queue(), on_topic_cb, subscriber_xt));
		ASSERT_EQ(BRICKS_SUCCESS, subscriber->subscribe(TEST_TOPIC, subscriber_decribe_topic_xt));
		

		this_thread::sleep_for(chrono::milliseconds(STABILIZATION_TIMEOUT));

		publish(publisher, TEST_TOPIC, publish_xt, all_snd_messages, NUM_OF_ITERATIONS, offset);

		this_thread::sleep_for(chrono::milliseconds(STABILIZATION_TIMEOUT));

		ASSERT_EQ(BRICKS_SUCCESS, subscriber->unsubscribe(TEST_TOPIC));

		this_thread::sleep_for(chrono::milliseconds(STABILIZATION_TIMEOUT));

		publish(publisher, TEST_TOPIC, publish_xt, all_snd_messages, NUM_OF_ITERATIONS, offset);

		this_thread::sleep_for(chrono::milliseconds(STABILIZATION_TIMEOUT));

	}
	
	ASSERT_EQ(all_rcv_messages.size(), NUM_OF_ITERATIONS);

	for (auto i : all_rcv_messages)
	{
		delete i;
	}
	
}