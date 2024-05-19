#include "gtest/gtest.h"
#include "pch.h"
#include "generic_tests.h"


using namespace bricks;
void
request_response_test_1(xtree_t* pxt, server_plugin_t* server, xtree_t* sxt, client_plugin_t* client, selector_t* selector, xtree_t* client_send_xt, xtree_t* server_response_xt)
{
	ASSERT_EQ(BRICKS_SUCCESS, server->init(selector->queue(),pxt));

	ASSERT_EQ(BRICKS_SUCCESS, client->init(selector->queue(), 1000,sxt));
	

	bool received_request = false;
	auto cb = [&](response_proxy_cb_t ch, buffer_t* buf,  xtree_t* xt)
		{
			
			auto cloned_xt = create_xtree();
			if (server_response_xt)
				clone_xtree(server_response_xt, xp_t(server_response_xt->get_root().value()), cloned_xt, xp_t(""));

			xt->release();
			buf->release();

			ch(BRICKS_SUCCESS, "pong", 4, cloned_xt);
			received_request = true;
		};

	ASSERT_EQ(BRICKS_SUCCESS, server->register_request_cb(cb,nullptr));

	ASSERT_EQ(BRICKS_SUCCESS, server->start());

	ASSERT_EQ(BRICKS_SUCCESS, client->start());

	bool received_response = false;
	ASSERT_EQ(BRICKS_SUCCESS, client->issue_request("ping", 5, [&](bricks_error_code_e, buffer_t* buf, xtree_t* xt)
		{
			if (buf) buf->release();
			if (xt) xt->release();
			received_response = true;
		},
		client_send_xt));

	int count = 0;
	while (count++ < 25  &&  ( received_request == false || received_response == false))
	{
		selector->poll(500);
	}
	


}	
