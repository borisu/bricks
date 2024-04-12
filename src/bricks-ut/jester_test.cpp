#include "pch.h"
#include "generic_tests.h"

using namespace std;
using namespace bricks;
using namespace bricks::plugins;

TEST(jester_case, publish_subscribe_test) {

	brick_uptr<brick_t> c (create_jester_context());

	brick_uptr<publisher_plugin_t> publisher(create_jester_publisher(c.get()));

	brick_uptr<subscriber_plugin_t> subscriber(create_jester_subscriber(c.get()));

	brick_uptr<cb_queue_t>  cb_q(create_callback_queue());

	brick_uptr<selector_t>  selector(create_selector());

	selector->init(cb_q.get());

	publish_subscribe_test_1(nullptr, publisher.get(), nullptr, subscriber.get(), selector.get());

}

