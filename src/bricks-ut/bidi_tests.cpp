#include "gtest/gtest.h"
#include "pch.h"
#include "generic_tests.h"

using namespace bricks;

void
bidi_test_1(xtree_t* xt1, bidi_plugin_t* p1, xtree_t* xt2, bidi_plugin_t* p2, selector_t* selector)
{
	ASSERT_EQ(BRICKS_SUCCESS, p1->init(
		selector->queue(),
		xt1));

	ASSERT_EQ(BRICKS_SUCCESS, p2->init(
		selector->queue(),
	xt2));

	int p1_events = 0;
	ASSERT_EQ(BRICKS_SUCCESS, p1->register_event_handler([&](buffer_t* buf, xtree_t* tree)
	{
			buf->release();
			tree->release();
			p1_events++;

	}));

	int p2_events = 0;
	ASSERT_EQ(BRICKS_SUCCESS, p2->register_event_handler([&](buffer_t* buf, xtree_t* tree)
		{
			buf->release();
			tree->release();
			p2_events++;
		}));

	ASSERT_EQ(BRICKS_SUCCESS, p1->start());
	ASSERT_EQ(BRICKS_SUCCESS, p2->start());

	ASSERT_EQ(BRICKS_SUCCESS, p1->send_event("ping1", 5, nullptr));
	ASSERT_EQ(BRICKS_SUCCESS, p2->send_event("ping2", 5, nullptr));

	int count = 0;
	while (count++ < 25 && (p1_events == 0 || p2_events == 0))
	{
		selector->poll(500);
	}

	ASSERT_EQ(1, p1_events);
	ASSERT_EQ(1, p2_events);
}
