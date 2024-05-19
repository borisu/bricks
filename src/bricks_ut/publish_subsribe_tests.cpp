#include "pch.h"
#include "gtest/gtest.h"
#include "generic_tests.h"

using namespace bricks;

#define TEST_TOPIC "bricks.test"
#define NUM_OF_MESSAGES 1000000

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
	list<msg_info> all_rcv_messages;

	/*
	* Prepare publisher
	*/
	ASSERT_EQ(BRICKS_SUCCESS, publisher->init(selector->queue(),publisher_xt));
	ASSERT_EQ(BRICKS_SUCCESS, publisher->add_topic(TEST_TOPIC, publisher_topic_xt));
	ASSERT_EQ(BRICKS_SUCCESS, publisher->start());


	/*
	* Prepare subscriber.
	*/

	auto on_topic_cb = [&](const string& topic, buffer_t* buf, xtree_t* xt) 
		{
			all_rcv_messages.push_back({topic, buf, xt});
			xt->release();
		};

	ASSERT_EQ(BRICKS_SUCCESS, subscriber->init(selector->queue(),on_topic_cb, subscriber_xt));
	ASSERT_EQ(BRICKS_SUCCESS, subscriber->subscribe(TEST_TOPIC, subscriber_topic_xt));
	ASSERT_EQ(BRICKS_SUCCESS, subscriber->start());

	/*
	* Let it sink in.
	*/
	this_thread::sleep_for(chrono::milliseconds(1000));

	for (int i = 0; i < 10000; i++)
	{
		/*
		* Publish the message
		*/
		string msg = "Another brick in the wall ";
		msg += std::to_string(i);
		all_snd_messages.push_back(msg);

		ASSERT_EQ(BRICKS_SUCCESS, publisher->publish(TEST_TOPIC, msg.c_str(), msg.length(), publish_xt));
	
		selector->poll(0);
	}

	this_thread::sleep_for(chrono::milliseconds(1000));

	ASSERT_EQ(all_snd_messages.size() , all_rcv_messages.size());
	
}