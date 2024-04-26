#include "gtest/gtest.h"
#include "pch.h"
#include "generic_tests.h"


using namespace bricks;
void
request_response_test_1(xtree_t* pxt, server_plugin_t* server, xtree_t* sxt, client_plugin_t* client, selector_t* selector)
{
	ASSERT_EQ(BRICKS_SUCCESS, server->init(selector->queue(), pxt));

	ASSERT_EQ(BRICKS_SUCCESS, client->init(selector->queue(), sxt));

	auto cb = [](response_channel_t ch, buffer_t* buf,  xtree_t* xt)
		{
			//printf("REQUEST\n");

			xt->release();
			buf->release();

			ch(BRICKS_INVALID_STATE, nullptr, 3, nullptr);

		};

	ASSERT_EQ(BRICKS_SUCCESS, server->register_request_handler(cb,nullptr));

	ASSERT_EQ(BRICKS_SUCCESS, server->start());

	ASSERT_EQ(BRICKS_SUCCESS, client->start());

	ASSERT_EQ(BRICKS_SUCCESS, client->request("ping",5));

	int count = 0;
	while (count++ < 25)
	{
		selector->poll(500);
	}

	this_thread::sleep_for(chrono::milliseconds(500000));

}	
