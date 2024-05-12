#pragma once
#include "bricks.h"

using namespace std;
using namespace bricks;


void
publish_subscribe_test_1(
	xtree_t* publisher_xt, 
	publisher_plugin_t* publisher, 
	xtree_t* subscriber_xt, 
	subscriber_plugin_t* subscriber, 
	selector_t* selector, 
	xtree_t* publisher_topic_xt = nullptr,
	xtree_t* subscriber_topic_xt = nullptr,
	xtree_t* publish_xt = nullptr,
	xtree_t* subscribers_xt = nullptr);

void
request_response_test_1(xtree_t* pxt, server_plugin_t* server, xtree_t* sxt, client_plugin_t* client, selector_t* selector, xtree_t* client_send_xt = nullptr,xtree_t* server_response_xt = nullptr);

void
bidi_test_1(xtree_t* xt1, bidi_plugin_t* p1, xtree_t* xt2, bidi_plugin_t* p2, selector_t* selector);