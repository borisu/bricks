#include "gtest/gtest.h"
#include "pch.h"

#include "xtree_api.h"
#include "kafka_api.h"
#include "services_api.h"

void msg_cb(bricks_error_code_e e, const char* msg)
{

}



TEST(kafka_case, publish_subscribe_test) {

	return;

	/* prepare producer */
	const char* kafka_producer_config_xml =
		"<configuration>"
		" <property name = \"bootstrap.servers\" value=\"127.0.0.1:29092\"/>"
		"</configuration>";
	publisher_service_t *kafka_producer_h = create_kafka_publisher();
	auto p_conf = create_xtree();
	ASSERT_EQ(BRICKS_SUCCESS, p_conf->load_from_xml(kafka_producer_config_xml));
	int delivered_counter = 0;
	ASSERT_EQ(BRICKS_SUCCESS, kafka_producer_h->init([&delivered_counter](void*, bricks_error_code_e, xtree_t&) 
		{
			printf("SENT");
			delivered_counter++;
		}, p_conf));
	ASSERT_EQ(BRICKS_SUCCESS, kafka_producer_h->register_topic("bricks.test"));


	/* prepare consumer */
	const char* kafka_consumer_config_xml =
		"<configuration>"
		" <property name = \"bootstrap.servers\" value=\"127.0.0.1:29092\"/>"
		" <property name = \"group.id\" value=\"mygroup\"/>"
		" <property name = \"auto.offset.reset\" value=\"earliest\"/>"
		"</configuration>";
	subscriber_service_t* kafka_subscriber_h = create_kafka_subscriber();
	auto c_conf = create_xtree();
	ASSERT_EQ(BRICKS_SUCCESS, c_conf->load_from_xml(kafka_consumer_config_xml));
	int received_counter = 0;
	ASSERT_EQ(BRICKS_SUCCESS, kafka_subscriber_h->init([&received_counter](void*, const char*, size_t, xtree_t&)
		{
			printf("RECEIVED");
			received_counter++;
		}, c_conf));
	ASSERT_EQ(BRICKS_SUCCESS, kafka_subscriber_h->register_topic("bricks.test"));


	const char* msg = "Another brick in the wall.";

	ASSERT_EQ(BRICKS_SUCCESS, kafka_subscriber_h->subscribe(nullptr));
	kafka_subscriber_h->poll(1000);

	ASSERT_EQ(BRICKS_SUCCESS, kafka_producer_h->publish("bricks.test", msg, strlen(msg), nullptr ));

	int poll_counter = 0;
	while (poll_counter < 10)
	{
		kafka_producer_h->poll(1000);
		kafka_subscriber_h->poll(1000);
	}
	
	destroy_kafka_publisher(kafka_producer_h);
	destroy_kafka_subscriber(kafka_subscriber_h);

	destroy_xtree(p_conf);
	destroy_xtree(c_conf);
}

