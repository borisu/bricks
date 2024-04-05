#pragma once
#include "bricks.h"
#include "rdkafka_common.h"

namespace bricks::plugins {

	BRICKSKAFKA_API publisher_plugin_t*
		create_kafka_publisher();

	BRICKSKAFKA_API subscriber_plugin_t*
		create_kafka_subscriber();

}

