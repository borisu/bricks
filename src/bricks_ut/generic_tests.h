#pragma once
#include "bricks.h"

using namespace std;
using namespace bricks;

struct msg_info
{
	string topic;

	buffer_t* buf = nullptr;
	xtree_t* xt   = nullptr;

	~msg_info()
	{
		if (buf)
			buf->release();
		if (xt)
			xt->release();
	}
};

#define NUM_OF_ITERATIONS 10
#define NUM_OF_TESTS 1
#define STABILIZATION_TIMEOUT 1000


void
publish_subscribe_test_2(
	publisher_plugin_t* publisher,
	subscriber_plugin_t* subscriber,
	selector_t* selector,
	xtree_t* xt = nullptr);

void
request_response_test_2(server_plugin_t* server, client_plugin_t* client, selector_t* selector, xtree_t* server_xt, xtree_t* client_xt, xtree_t* issue_xt);

void
request_response_test_timeout(server_plugin_t* server, client_plugin_t* client, selector_t* selector, xtree_t* xt);

void
bidi_test_2(bidi_plugin_t* p1, bidi_plugin_t* p2, selector_t* selector, xtree_t* xt1, xtree_t* xt2);
