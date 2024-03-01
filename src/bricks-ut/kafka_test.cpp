#include "gtest/gtest.h"
#include "pch.h"

#include "xtree_api.h"
#include "kafka_api.h"
#include "services_api.h"

TEST(kafka_case, publish_subscribe_test) {

	bricks_handle_t kafka_config_h =
		bricks_create_xtree_from_xml_string(
			"<config>"
			"<property name = \"value\" / >"
			"</ config>"
		);

	EXPECT_NE(kafka_config_h, 0);
	
	bricks_kafka_init(kafka_config_h);

	bricks_kafka_register_service("topic1");

	bricks_publish("topic1", "All in all, it's just another brick in the wall!");
}