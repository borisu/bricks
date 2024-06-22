#include "pch.h"
#include "gtest/gtest.h"
#include "generic_tests.h"
#include "utils.h"


using namespace bricks;

void
request_response_test_2(server_plugin_t* server, client_plugin_t* client, selector_t* selector, xtree_t* server_xt, xtree_t* client_xt, xtree_t* issue_xt)
{
	brick_uptr<brick_t> c(create_poller(BRICKS_DEFAULT_CLIENT_TIMEOUT, selector));

	
	int req_count = 0;
	auto cb = [&](response_proxy_cb_t proxy, buffer_t* buf, xtree_t* request_xt)
		{
			printf("pubsusb_server_t::REQUEST HANDLER\n");

			buf->release();

			proxy(BRICKS_SUCCESS, "pong", 5, request_xt);

			req_count++;
		};
	ASSERT_EQ(BRICKS_SUCCESS, server->init(selector->queue(), cb, server_xt));
	ASSERT_EQ(BRICKS_SUCCESS, client->init(selector->queue(), BRICKS_DEFAULT_CLIENT_TIMEOUT, client_xt));

	this_thread::sleep_for(chrono::milliseconds(STABILIZATION_TIMEOUT));

	int rsp_count = 0;
	for (int i = 0; i < NUM_OF_ITERATIONS; i++)
	{
		ASSERT_EQ(BRICKS_SUCCESS, client->issue_request("ping", 5, [&](bricks_error_code_e, buffer_t* buf, xtree_t* xt)
			{
				if (buf) buf->release();
				if (xt) xt->release();
				rsp_count++;
			},
			issue_xt));
	}

	this_thread::sleep_for(chrono::milliseconds(STABILIZATION_TIMEOUT));

	ASSERT_EQ(rsp_count, NUM_OF_ITERATIONS);
	ASSERT_EQ(req_count, NUM_OF_ITERATIONS);

}

void
request_response_test_timeout(server_plugin_t* server, client_plugin_t* client, selector_t* selector, xtree_t* xt)
{
	brick_uptr<brick_t> c(create_poller(BRICKS_DEFAULT_CLIENT_TIMEOUT, selector));

	int req_count = 0;
	auto cb = [&](response_proxy_cb_t proxy, buffer_t* buf, xtree_t* request_xt)
		{
			buf->release();

			// reply nothing

			req_count++;
		};
	ASSERT_EQ(BRICKS_SUCCESS, server->init(selector->queue(), cb, xt));

	ASSERT_EQ(BRICKS_SUCCESS, client->init(selector->queue(), BRICKS_DEFAULT_CLIENT_TIMEOUT, xt));

	this_thread::sleep_for(chrono::milliseconds(STABILIZATION_TIMEOUT));

	int rsp_count = 0;
	for (int i = 0; i < NUM_OF_ITERATIONS; i++)
	{
		ASSERT_EQ(BRICKS_SUCCESS, client->issue_request("ping", 5, [&](bricks_error_code_e err, buffer_t* buf, xtree_t* xt)
			{
				if (buf) buf->release();
				if (xt) xt->release();
				rsp_count++;
			},
			xt));
	}

	this_thread::sleep_for(chrono::milliseconds(NUM_OF_ITERATIONS * BRICKS_DEFAULT_CLIENT_TIMEOUT));

	ASSERT_EQ(rsp_count, NUM_OF_ITERATIONS);
	ASSERT_EQ(req_count, NUM_OF_ITERATIONS);

}

