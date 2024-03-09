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

	service_t *kafka_config_h = service_kafka_create();

	auto conf = create_xtree();

	EXPECT_EQ(BRICKS_SUCCESS, conf->load_from_xml(kafka_config_xml));
	EXPECT_EQ(BRICKS_SUCCESS, kafka_config_h->init(*conf));


	service_kafka_destroy(kafka_config_h);
	
	
}