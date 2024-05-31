#include "pch.h"
#include "gtest/gtest.h"
#include "generic_tests.h"
#include "utils.h"

using namespace bricks;

#define TEST_TOPIC "bricks.test.16"

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
publish_subscribe_test_1(
	xtree_t *publisher_xt, 
	publisher_plugin_t* publisher, 
	xtree_t* subscriber_xt, 
	subscriber_plugin_t* subscriber, 
	selector_t *selector,
	xtree_t* publisher_topic_xt,
	xtree_t* subscriber_topic_xt,
	xtree_t* publish_xt,
	xtree_t* subscribe_xt)
{

	list<string>   all_snd_messages;
	list<msg_info*> all_rcv_messages;

	{
		int offset = 0;

		brick_uptr<brick_t> c(create_poller(BRICKS_DEFAULT_CLIENT_TIMEOUT, selector));

		/*
		* Prepare subscriber.
		*/
		auto on_topic_cb = [&](const string& topic, buffer_t* buf, xtree_t* xt)
			{
				all_rcv_messages.push_back(new msg_info{ topic, buf, xt });
			};

		/*
		* Prepare publisher
		*/

		printf("$$$ === Init Publisher ===\n");

		ASSERT_EQ(BRICKS_SUCCESS, publisher->init(selector->queue(), publisher_xt));
		ASSERT_EQ(BRICKS_SUCCESS, publisher->add_topic(TEST_TOPIC, publisher_topic_xt));
		ASSERT_EQ(BRICKS_SUCCESS, publisher->start());

		printf("$$$ === Waiting  0  ===\n");

		//this_thread::sleep_for(chrono::milliseconds(STABILIZATION_TIMEOUT));

		printf("$$$ === Init Subscribe  ===\n");

		ASSERT_EQ(BRICKS_SUCCESS, subscriber->init(selector->queue(), on_topic_cb, subscriber_xt));
		ASSERT_EQ(BRICKS_SUCCESS, subscriber->subscribe(TEST_TOPIC, subscriber_topic_xt));
		ASSERT_EQ(BRICKS_SUCCESS, subscriber->start());

		printf("$$$ === Waiting  1  (long) ===\n");

		this_thread::sleep_for(chrono::milliseconds(6*STABILIZATION_TIMEOUT));

		printf("$$$ === Publishing 1 ===\n");

		publish(publisher, TEST_TOPIC, publish_xt, all_snd_messages, NUM_OF_ITERATIONS, offset);

		printf("$$$ === Waiting  2  (short) ===\n");

		this_thread::sleep_for(chrono::milliseconds( STABILIZATION_TIMEOUT));

		printf("=== Unsubscribing 1 ===\n");

		ASSERT_EQ(BRICKS_SUCCESS, subscriber->unsubscribe(TEST_TOPIC));

		printf("$$$ === Waiting  2 (long)  ===\n");

		this_thread::sleep_for(chrono::milliseconds(5 * STABILIZATION_TIMEOUT));

		printf("$$$ === Publishing 2 ===\n");
		publish(publisher, TEST_TOPIC, publish_xt, all_snd_messages, NUM_OF_ITERATIONS, offset);

		printf("$$$ === Waiting  3  ===\n");
		this_thread::sleep_for(chrono::milliseconds(STABILIZATION_TIMEOUT));

		printf("$$$ === Exiting ===\n");
	}
	
	ASSERT_EQ(all_rcv_messages.size(), NUM_OF_ITERATIONS);

	for (auto i : all_rcv_messages)
	{
		delete i;
	}
	
}