#include "pch.h"
#include "gtest/gtest.h"
#include "generic_tests.h"
#include "utils.h"


using namespace bricks;
void
request_response_test_1(xtree_t* pxt, server_plugin_t* server, xtree_t* sxt, client_plugin_t* client, selector_t* selector, xtree_t* client_send_xt, xtree_t* server_response_xt)
{
	brick_uptr<brick_t> c(create_poller(BRICKS_DEFAULT_CLIENT_TIMEOUT, selector));

	ASSERT_EQ(BRICKS_SUCCESS, server->init(selector->queue(),pxt));
	int req_count = 0;
	auto cb = [&](response_proxy_cb_t proxy, buffer_t* buf, xtree_t* xt)
		{

			auto cloned_xt = create_xtree();

			if (server_response_xt)
				clone_xtree(server_response_xt, xp_t(server_response_xt->get_root().value()), cloned_xt, xp_t(""));

			xt->release();
			buf->release();

			proxy(BRICKS_SUCCESS, "pong", 4, cloned_xt);
			req_count++;
		};

	ASSERT_EQ(BRICKS_SUCCESS, server->register_request_cb(cb, nullptr));
	ASSERT_EQ(BRICKS_SUCCESS, server->start());


	ASSERT_EQ(BRICKS_SUCCESS, client->init(selector->queue(), BRICKS_DEFAULT_CLIENT_TIMEOUT, sxt));
	ASSERT_EQ(BRICKS_SUCCESS, client->start());

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
			client_send_xt));
	}

	this_thread::sleep_for(chrono::seconds(3600));

	ASSERT_EQ(rsp_count, NUM_OF_ITERATIONS);
	ASSERT_EQ(req_count, NUM_OF_ITERATIONS);
	

}	
