#include "gtest/gtest.h"
#include "pch.h"
#include "generic_tests.h"


using namespace bricks;
void
request_response_test_1(xtree_t* pxt, server_plugin_t* server, xtree_t* sxt, client_plugin_t* client, selector_t* selector)
{
	ASSERT_EQ(BRICKS_SUCCESS, server->init(selector->queue(), pxt));

	ASSERT_EQ(BRICKS_SUCCESS, client->init(selector->queue(), sxt));

	auto cb = [](bricks_handle_t, buffer_t*, const xtree_t*) 
		{
			printf("REQUEST\n");
		};

	ASSERT_EQ(BRICKS_SUCCESS, server->register_request_handler(cb,nullptr));

	ASSERT_EQ(BRICKS_SUCCESS, server->start());

	ASSERT_EQ(BRICKS_SUCCESS, client->start());

	ASSERT_EQ(BRICKS_SUCCESS, client->request("ping",5));

	this_thread::sleep_for(chrono::milliseconds(500000));

}	
