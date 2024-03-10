#include "gtest/gtest.h"
#include "pch.h"

#include "xtree_api.h"
#include "kafka_api.h"
#include "services_api.h"

void msg_cb(bricks_error_code_e e, const char* msg)
{

}



TEST(kafka_case, publish_subscribe_test) {

	const char* kafka_producer_config_xml =
		"<configuration>"
		" <property name = \"bootstrap.servers\" value=\"127.0.0.1:29092\"/>"
		"</configuration>";

	const char* kafka_conusmer_config_xml =
		"<configuration>"
		" <property name = \"bootstrap.servers\" value=\"127.0.0.1:29092\"/>"
		" <property name = \"group.id\" value=\"mygroup\"/>"
		"</configuration>";

	service_t *kafka_service_h = service_kafka_create();

	auto p_conf = create_xtree();
	auto c_conf = create_xtree();

	ASSERT_EQ(BRICKS_SUCCESS, p_conf->load_from_xml(kafka_producer_config_xml));
	ASSERT_EQ(BRICKS_SUCCESS, c_conf->load_from_xml(kafka_conusmer_config_xml));

	ASSERT_EQ(BRICKS_SUCCESS, kafka_service_h->init());

	const char *msg = "Another brick in the wall.";
	buffer_t buf (msg, msg + strlen(msg));

	ASSERT_EQ(BRICKS_SUCCESS, kafka_service_h->register_publisher("bricks.test",p_conf));
	ASSERT_EQ(BRICKS_SUCCESS, kafka_service_h->publish("bricks.test", buf, nullptr, nullptr));

	ASSERT_EQ(BRICKS_SUCCESS, kafka_service_h->subscribe("bricks.test", nullptr, c_conf));
	
	service_kafka_destroy(kafka_service_h);

	destroy_xtree(p_conf);
	destroy_xtree(c_conf);
}
