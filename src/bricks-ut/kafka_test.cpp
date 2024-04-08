#include "pch.h"
#include "generic_tests.h"

using namespace std;
using namespace bricks;
using namespace bricks::plugins;

TEST(kafka_case, publish_subscribe_test) {

	brick_ptr<xtree_t> p_xt(
		create_xtree_from_xml(
			"<configuration>"
			" <property name = \"bootstrap.servers\" value=\"127.0.0.1:29092\"/>"
			"</configuration>"
		), brick_destroy);


	brick_ptr<xtree_t> s_xt(
		create_xtree_from_xml(
			"<configuration>"
			" <property name = \"bootstrap.servers\" value=\"127.0.0.1:29092\"/>"
			" <property name = \"group.id\" value=\"mygroup\"/>"
			" <property name = \"auto.offset.reset\" value=\"latest\"/>"
			"</configuration>"
		));

	brick_ptr<publisher_plugin_t>  publisher(create_kafka_publisher());
	publisher->name("kafka:publisher_1");


	brick_ptr<subscriber_plugin_t>  subscriber(create_kafka_subscriber());
	subscriber->name("kafka:subscriber_1");

	brick_ptr<cb_queue_t>  cb_q(create_callback_queue());

	brick_ptr<selector_t>  selector(create_selector());

	selector->init(cb_q.get());
	
	publish_subscribe_test_1(p_xt.get(), publisher.get(), s_xt.get(), subscriber.get(), selector.get());*/

}

