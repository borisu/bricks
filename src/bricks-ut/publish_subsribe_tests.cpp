#include "gtest/gtest.h"
#include "pch.h"

using namespace bricks;

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

		}, pxt));

	ASSERT_EQ(BRICKS_SUCCESS, publisher->register_topic("bricks.test"));
	ASSERT_EQ(BRICKS_SUCCESS, publisher->start());

	/*
		prepare subscriber
	*/
	int       received_counter = 0;
	buffer_t  received_buf;

	ASSERT_EQ(BRICKS_SUCCESS, subscriber->init(
		selector->queue(),
		[&](void*, const char* buf, size_t size, xtree_t* xt)
		{
			printf("RECEIVED");
			received_counter++;
			received_buf.assign(buf, buf + size);
		}, sxt));

	ASSERT_EQ(BRICKS_SUCCESS, subscriber->register_topic("bricks.test"));
	ASSERT_EQ(BRICKS_SUCCESS, subscriber->subscribe(nullptr));


	/* 
		publish message
	*/
	const char* msg = "Another brick in the wall.";
	int publisher_opaque = 1001;
	ASSERT_EQ(BRICKS_SUCCESS, publisher->publish("bricks.test", msg, strlen(msg), &publisher_opaque));

	int count = 0;
	while (count++ < 10 || (received_counter == 1  && delivered_counter == 1))
	{
		selector->poll(500);
	}
	

	ASSERT_EQ(1, received_counter);
	ASSERT_EQ(1, delivered_counter);
	
}