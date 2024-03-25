#include "pch.h"
#include "generic_tests.h"

using namespace std;
using namespace bricks;




TEST(kafka_case, publish_subscribe_test) {

	unique_ptr<xtree_t, bricks_destroyer> p_xt (
		create_xtree_from_xml(
			"<configuration>"
			" <property name = \"bootstrap.servers\" value=\"127.0.0.1:29092\"/>"
			"</configuration>"
		));

	unique_ptr<xtree_t, bricks_destroyer> s_xt ( 
		create_xtree_from_xml(
			"<configuration>"
			" <property name = \"bootstrap.servers\" value=\"127.0.0.1:29092\"/>"
			" <property name = \"group.id\" value=\"mygroup\"/>"
			" <property name = \"auto.offset.reset\" value=\"latest\"/>"
			"</configuration>"
		));


	unique_ptr<publisher_plugin_t, bricks_destroyer>  publisher(create_kafka_publisher());

	unique_ptr<subscriber_plugin_t, bricks_destroyer>  subscriber(create_kafka_subscriber());

	unique_ptr<cb_queue_t, bricks_destroyer>  cb_q(create_callback_queue());

	unique_ptr<selector_t, bricks_destroyer>  selector(create_selector());

	selector->init(cb_q.get());
	
	publish_subscribe_test_1(p_xt.get(), publisher.get(), s_xt.get(), subscriber.get(), selector.get());

}

