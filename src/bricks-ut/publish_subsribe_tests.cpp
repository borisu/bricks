#include "gtest/gtest.h"
#include "pch.h"

using namespace bricks;

#define TEST_TOPIC "bricks.test"

void
publish_subscribe_test_1(xtree_t *pxt, publisher_plugin_t* publisher, xtree_t* sxt, subscriber_plugin_t* subscriber, selector_t *selector)
{
	/*
		prepare publisher 
	*/
	int delivered_counter             = 0;
	bricks_error_code_e delivered_err = BRICKS_NOT_SUPPORTED;
	xtree_t* delivered_xt             = nullptr;
	void* delivered_opaque            = nullptr;

	ASSERT_EQ(BRICKS_SUCCESS, publisher->init(
		selector->queue(),
		[&](void* opaque, bricks_error_code_e err, xtree_t* xt)
		{
			printf("SENT");
			delivered_counter++;
			delivered_xt 	 = xt;
			delivered_err	 = err;
			delivered_opaque = opaque;
			xt->release();

		}, pxt));

	ASSERT_EQ(BRICKS_SUCCESS, publisher->register_topic(TEST_TOPIC));
	ASSERT_EQ(BRICKS_SUCCESS, publisher->start());

	/*
		prepare subscriber
	*/
	int       received_counter = 0;
	vector_t  received_buf;
	ASSERT_EQ(BRICKS_SUCCESS, subscriber->init(
		selector->queue(),
		[&](void*, buffer_t *buf, xtree_t* xt)
		{
			printf("RECEIVED");
			received_counter++;
			received_buf.resize(buf->size());
			std::memcpy(received_buf.data(), buf->data(), buf->size());
			buf->release();
			xt->release();

		}, sxt));

	ASSERT_EQ(BRICKS_SUCCESS, subscriber->register_topic(TEST_TOPIC));
	ASSERT_EQ(BRICKS_SUCCESS, subscriber->register_topic("dummy"));
	ASSERT_EQ(BRICKS_SUCCESS, subscriber->start());

	/* 
		publish message
	*/
	const char* msg = "Another brick in the wall.";
	int publisher_opaque = 1001;
	ASSERT_EQ(BRICKS_SUCCESS, publisher->publish(TEST_TOPIC, msg, strlen(msg), &publisher_opaque));

	int count = 0;
	while (count++ < 25 || (received_counter > 1 && delivered_counter > 1))
	{
		selector->poll(500);
	}
	
	ASSERT_LE(1, received_counter);	
	ASSERT_LE(1, delivered_counter);
	
}