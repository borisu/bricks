#include "pch.h"
#include "gtest/gtest.h"
#include "generic_tests.h"

using namespace bricks;

#define TEST_TOPIC "bricks.test"

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
	
	ASSERT_EQ(BRICKS_SUCCESS, publisher->init(
		selector->queue(),
	    publisher_xt));

	ASSERT_EQ(BRICKS_SUCCESS, publisher->register_topic(TEST_TOPIC, publisher_topic_xt));
	ASSERT_EQ(BRICKS_SUCCESS, publisher->start());

	/*
		prepare subscriber
	*/
	int       received_counter = 0;
	vector_t  received_buf;
	ASSERT_EQ(BRICKS_SUCCESS, subscriber->init(
		selector->queue(),
		[&](const string& ,buffer_t *buf, xtree_t* xt)
		{
			received_counter++;

			if (buf->size() > 0)
			{
				received_buf.resize(buf->size());
				std::memcpy(received_buf.data(), buf->data(), buf->size());

			}

			buf->release();
			xt->release();

		}, subscriber_xt));

	ASSERT_EQ(BRICKS_SUCCESS, subscriber->register_topic(TEST_TOPIC, subscriber_topic_xt));
	ASSERT_EQ(BRICKS_SUCCESS, subscriber->start());

	this_thread::sleep_for(chrono::milliseconds(4000));

	/* 
		publish message
	*/
	const char* msg = "Another brick in the wall.";
	ASSERT_EQ(BRICKS_SUCCESS, publisher->publish(TEST_TOPIC, msg, strlen(msg), publish_xt));

	int count = 0;
	while (count++ < 25 && (received_counter < 1))
	{
		selector->poll(500);
	}
	
	ASSERT_EQ(1, received_counter);	
	
	std::string str(received_buf.begin(), received_buf.end());

	ASSERT_STREQ(msg, str.c_str());
	
}