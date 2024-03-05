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

	bricks_handle_t kafka_config_h =
		bricks_create_xtree_from_xml_string(kafka_config_xml);

	EXPECT_NE(kafka_config_h, 0);
	
	bricks_kafka_init(kafka_config_h, msg_cb);

	//bricks_kafka_register_service("topic1");

	//bricks_publish("topic1", "All in all, it's just another brick in the wall!");
}