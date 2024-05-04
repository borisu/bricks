#include "pch.h"
#include "generic_tests.h"

using namespace std;
using namespace bricks;
using namespace bricks::plugins;

TEST(kafka_case, publish_subscribe_test) {
	{

		brick_uptr<xtree_t> publisher_xt(
			create_xtree_from_xml(
				"<bricks>"
				" <rdkafka>"
				"  <rdlog enabled=\"true\"/>"
				"  <producer name=\"producer1\">"
				"   <configuration>"
				"    <property name = \"bootstrap.servers\" value=\"127.0.0.1:29092\"/>"
				"   </configuration>"
				"  </producer>"
				" </rdkafka>"
				"</bricks>"
			));

		brick_uptr<xtree_t> publisher_topic_xt(
			create_xtree_from_xml(
				"<bricks>"
				" <rdkafka>"
				"  <producer>"
				"   <topic>"
				"    <configuration>"
				"     <property name = \"acks\" value=\"all\"/>"
				"    </configuration>"
				"   </topic>"
				"  </producer>"
				" </rdkafka>"
				"</bricks>"
			));

		brick_uptr<xtree_t> publish_xt(
			create_xtree_from_xml(
				"<bricks>"
				" <rdkafka>"
				"  <producer>"
				"   <key>YnJpY2tzLnRlc3Qua2V5</key>" // bricks.test.key (base64)
				"  </producer>"
				" </rdkafka>"
				"</bricks>"
			));

		brick_uptr<xtree_t> subscriber_xt(
			create_xtree_from_xml(
				"<bricks>"
				" <rdkafka>"
				"  <rdlog enabled=\"true\"/>"
				"  <consumer name=\"consumer1\">"
				"   <configuration>"
				"    <property name = \"bootstrap.servers\" value=\"127.0.0.1:29092\"/>"
				"    <property name = \"group.id\" value=\"bricks.test.group\"/>"
				"    <property name = \"enable.auto.commit\" value=\"%s:false\"/>"
				"    <property name = \"auto.offset.reset\" value=\"latest\"/>"
				"   </configuration>"
				"  </consumer>"
				" </rdkafka>"
				"</bricks>"
			));

		brick_uptr<xtree_t> subscriber_topic_xt(
			create_xtree_from_xml(
				"<bricks>"
				" <rdkafka>"
				"  <consumer>"
				"   <topic>"
				"     <partition value=\"0\"/>"
				"   </topic>"
				"  </consumer>"
				" </rdkafka>"
				"</bricks>"
			));

		brick_uptr<publisher_plugin_t>  publisher(create_kafka_publisher());

		brick_uptr<subscriber_plugin_t>  subscriber(create_kafka_subscriber());

		brick_uptr<cb_queue_t>  cb_q(create_callback_queue());

		brick_uptr<selector_t>  selector(create_selector());

		selector->init(cb_q.get());

		publish_subscribe_test_1(
			publisher_xt.get(),
			publisher.get(),
			subscriber_xt.get(),
			subscriber.get(),
			selector.get(),
			publisher_topic_xt.get(),
			subscriber_topic_xt.get(),
			publish_xt.get());
	}

}

