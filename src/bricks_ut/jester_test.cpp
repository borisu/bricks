#include "pch.h"
#include "generic_tests.h"
#include "jester_plugin.h"

using namespace std;
using namespace bricks;
using namespace bricks::plugins;

TEST(jester_case, publish_subscribe_test) {

	brick_uptr<brick_t> c (create_jester_pubsub_context());

	brick_uptr<publisher_plugin_t> publisher(create_jester_publisher(c.get()));

	brick_uptr<subscriber_plugin_t> subscriber(create_jester_subscriber(c.get()));

	brick_uptr<cb_queue_t>  cb_q(create_callback_queue());

	brick_uptr<selector_t>  selector(create_selector());

	selector->init(cb_q.get());

	publish_subscribe_test_1(nullptr, publisher.get(), nullptr, subscriber.get(), selector.get());

}

TEST(jester_case, bidi_test) {

	brick_uptr<brick_t> c(create_jester_bidi_context());

	brick_uptr<bidi_plugin_t> p1(create_jester_bidi(c.get(), 0));

	brick_uptr<bidi_plugin_t> p2(create_jester_bidi(c.get(), 1));

	brick_uptr<cb_queue_t>  cb_q(create_callback_queue());

	brick_uptr<selector_t>  selector(create_selector());

	selector->init(cb_q.get());

	bidi_test_1(nullptr, p1.get(), nullptr, p2.get(), selector.get());

}

TEST(jester_case, reqrep_test) {

	brick_uptr<brick_t> c(create_jester_reqrep_ctx());

	brick_uptr<server_plugin_t> server(create_jester_server(c.get()));

	brick_uptr<client_plugin_t> client(create_jester_client(c.get()));

	brick_uptr<cb_queue_t>  cb_q(create_callback_queue());

	brick_uptr<selector_t>  selector(create_selector());

	selector->init(cb_q.get());

	request_response_test_1(nullptr, server.get(), nullptr, client.get(), selector.get());

}
