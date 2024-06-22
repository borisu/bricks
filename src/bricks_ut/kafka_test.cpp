#include "pch.h"

#ifndef NO_KAFKA_PLUGIN

#include "generic_tests.h"
#include "rdkafka_plugin.h"

using namespace std;
using namespace bricks;
using namespace bricks::plugins;

TEST(kafka_case, publish_subscribe_test) {

	for (int i = 0; i < NUM_OF_TESTS ; i++)
	{
		char hex_buffer[64];
	    generate_random_64hex_str(hex_buffer);


		brick_uptr<xtree_t> options_xt(
			create_xtree_from_xml(
				"<bricks>"
				" <rdkafka>"
				"  <publisher name=\"rdkafka_producer1\">"
				"   <rdlog enabled=\"true\"/>"
				"   <methods>"
				"	 <init>"
				"     <configuration>"
				"      <property name = \"bootstrap.servers\" value=\"127.0.0.1:29092\"/>"
				//"      <property name = \"debug\" value=\"all\"/>"
				"     </configuration>"
				"    </init>"
				"    <describe_topic>"
				"     <configuration>"
				//"      <property name = \"acks\" value=\"all\"/>"
				"     </configuration>"
				"    </describe_topic>"
				"    <publish>"
				//"      <key>YnJpY2tzLnRlc3Qua2V5</key>" 
				"    </publish>"
				"   </methods>"
				"  </publisher>"
				"  <subscriber name=\"rdkafka_consumer1\">"
				"   <rdlog enabled=\"true\"/>"
				"   <methods>"
				"	 <init>"
				"     <configuration>"
				"      <property name = \"bootstrap.servers\" value=\"127.0.0.1:29092\"/>"
			//	"      <property name = \"group.id\" value=\"bricks.test.group.777\"/>"
			//	"      <property name = \"debug\" value=\"all\"/>"
				"     </configuration>"
				"    </init>"
				"    <subscribe>"
				"     <partition value=\"0\"/>"
				"    </subscribe>"
				"   </methods>"
				"  </subscriber>"
				" </rdkafka>"
				"</bricks>"
			)
		);

		// we need to randomize kafka groups to exclude interference between tests as 
		// in group the offset of the group is persistent
		auto node = options_xt->add_node("/bricks/rdkafka/subscribers/methods/init/property",true);
		options_xt->set_property_value(node.value(),"name", "group.id");
		options_xt->set_property_value(node.value(),"value", "bricks.test.group.888");
		

		brick_uptr<cb_queue_t>  cb_q(create_callback_queue());

		brick_uptr<selector_t>  selector(create_selector());

		brick_uptr<publisher_plugin_t>  publisher(create_kafka_publisher());

		brick_uptr<subscriber_plugin_t>  subscriber(create_kafka_subscriber());

		selector->init(cb_q.get());

		publish_subscribe_test_2(
			publisher.get(),
			subscriber.get(),
			selector.get(),
			options_xt.get());

	}

}

#endif // !NO_KAFKA_PLUGIN 