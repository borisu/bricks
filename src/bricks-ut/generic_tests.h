#pragma once
#include "bricks.h"

using namespace std;
using namespace bricks;


void
publish_subscribe_test_1(xtree_t* pxt, publisher_plugin_t* publisher, xtree_t* sxt, subscriber_plugin_t* subscriber, selector_t* selector);


void
request_response_test_1(xtree_t* pxt, server_plugin_t* server, xtree_t* sxt, client_plugin_t* client, selector_t* selector);
