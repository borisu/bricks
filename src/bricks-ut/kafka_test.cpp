#include "gtest/gtest.h"
#include "pch.h"

#include "xtree_api.h"
#include "kafka_api.h"
#include "services_api.h"

void msg_cb(bricks_error_code_e e, const char* msg)
{

}

const char* kafka_config_xml =
"<configuration>"
" <property name = \"bootstrap.servers\" value=\"127.0.0.1:29092\"/>"
"</configuration>";

TEST(kafka_case, publish_subscribe_test) {

	service_t *kafka_service_h = service_kafka_create();

	auto conf = create_xtree();

	ASSERT_EQ(BRICKS_SUCCESS, conf->load_from_xml(kafka_config_xml));
	ASSERT_EQ(BRICKS_SUCCESS, kafka_service_h->init());

	const char *msg = "Another brick in the wall.";
	buffer_t buf (msg, msg + strlen(msg));

	ASSERT_EQ(BRICKS_SUCCESS, kafka_service_h->register_publisher("bricks.test",conf));
	ASSERT_EQ(BRICKS_SUCCESS, kafka_service_h->publish("bricks.test", buf, nullptr, conf));

	ASSERT_EQ(BRICKS_SUCCESS, kafka_service_h->subscribe("bricks.test", nullptr, conf));
	
	service_kafka_destroy(kafka_service_h);

}