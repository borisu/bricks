#pragma once
#include "bricks.h"

using namespace std;
using namespace bricks;


void
publish_subscribe_test_1(xtree_t* pxt, publisher_plugin_t* publisher, xtree_t* sxt, subscriber_plugin_t* subscriber, selector_t* selector);

void
request_response_test_1(xtree_t* pxt, server_plugin_t* server, xtree_t* sxt, client_plugin_t* client, selector_t* selector);

void
bidi_test_1(xtree_t* xt1, bidi_plugin_t* p1, xtree_t* xt2, bidi_plugin_t* p2, selector_t* selector);
