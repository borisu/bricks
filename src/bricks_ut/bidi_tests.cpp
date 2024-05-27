#include "pch.h"
#include "gtest/gtest.h"
#include "generic_tests.h"
#include "utils.h"

using namespace bricks;

void
bidi_test_1(xtree_t* xt1, bidi_plugin_t* p1, xtree_t* xt2, bidi_plugin_t* p2, selector_t* selector)
{
	brick_uptr<brick_t> c(create_poller(BRICKS_DEFAULT_CLIENT_TIMEOUT, selector));

	ASSERT_EQ(BRICKS_SUCCESS, p1->init(
		selector->queue(),
		xt1));

	ASSERT_EQ(BRICKS_SUCCESS, p2->init(
		selector->queue(),
	xt2));

	int p1_events = 0;
	ASSERT_EQ(BRICKS_SUCCESS, p1->register_event_cb([&](buffer_t* buf, xtree_t* tree)
	{
			if (buf) buf->release();
			if (tree) tree->release();
			p1_events++;

	}));

	int p2_events = 0;
	ASSERT_EQ(BRICKS_SUCCESS, p2->register_event_cb([&](buffer_t* buf, xtree_t* tree)
		{
			if (buf) buf->release();
			if (tree) tree->release();
			p2_events++;
		}));

	ASSERT_EQ(BRICKS_SUCCESS, p1->start());
	ASSERT_EQ(BRICKS_SUCCESS, p2->start());

	for (int i = 0; i < NUM_OF_ITERATIONS; i++)
	{
		ASSERT_EQ(BRICKS_SUCCESS, p1->send_event("ping1", 5, nullptr));
		ASSERT_EQ(BRICKS_SUCCESS, p2->send_event("ping2", 5, nullptr));
	}
	
	this_thread::sleep_for(chrono::milliseconds(STABILIZATION_TIMEOUT));

	ASSERT_EQ(NUM_OF_ITERATIONS, p1_events);
	ASSERT_EQ(NUM_OF_ITERATIONS, p2_events);
}
