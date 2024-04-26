#include "pch.h"
#include "generic_tests.h"

using namespace std;
using namespace bricks;
using namespace bricks::plugins;

TEST(kafka_case, publish_subscribe_test) {

	brick_uptr<xtree_t> p_xt (
		create_xtree_from_xml(
			"<rdkafka>"
			" <plugin name=\"subscriber1\"/>"
			" <configuration>"
			"  <property name = \"bootstrap.servers\" value=\"127.0.0.1:29092\"/>"
			" </configuration>"
			"</rdkafka>"
		));

	brick_uptr<xtree_t> s_xt(
		create_xtree_from_xml(
			"<rdkafka>"
			" <plugin name=\"subscriber1\"/>"
			" <configuration>"
			"  <property name = \"bootstrap.servers\" value=\"127.0.0.1:29092\"/>"
			"  <property name = \"group.id\" value=\"mygroup\"/>"
			"  <property name = \"auto.offset.reset\" value=\"latest\"/>"
			" </configuration>"
			"</rdkafka>"
		));

	brick_uptr<publisher_plugin_t>  publisher(create_kafka_publisher());
	
	brick_uptr<subscriber_plugin_t>  subscriber(create_kafka_subscriber());

	brick_uptr<cb_queue_t>  cb_q(create_callback_queue());

	brick_uptr<selector_t>  selector(create_selector());

	selector->init(cb_q.get());
	
	publish_subscribe_test_1(p_xt.get(), publisher.get(), s_xt.get(), subscriber.get(), selector.get());

}

