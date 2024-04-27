#include "gtest/gtest.h"
#include "pch.h"
#include "generic_tests.h"


using namespace bricks;
void
request_response_test_1(xtree_t* pxt, server_plugin_t* server, xtree_t* sxt, client_plugin_t* client, selector_t* selector)
{
	ASSERT_EQ(BRICKS_SUCCESS, server->init(selector->queue(), pxt));

	ASSERT_EQ(BRICKS_SUCCESS, client->init(selector->queue(), sxt));

	bool received_request = false;
	auto cb = [&](response_channel_t ch, buffer_t* buf,  xtree_t* xt)
		{
			xt->release();
			buf->release();
			ch(BRICKS_SUCCESS, "pong", 4, nullptr);
			received_request = true;
		};

	ASSERT_EQ(BRICKS_SUCCESS, server->register_request_handler(cb,nullptr));

	ASSERT_EQ(BRICKS_SUCCESS, server->start());

	ASSERT_EQ(BRICKS_SUCCESS, client->start());

	bool received_response = false;
	ASSERT_EQ(BRICKS_SUCCESS, client->request("ping", 5, [&](bricks_error_code_e, buffer_t* buf, xtree_t* xt)
		{
			buf->release();
			xt->release();
			received_response = true;
		}));

	int count = 0;
	while (count++ < 25  &&  ( received_request == false || received_response == false))
	{
		selector->poll(500);
	}
	


}	
