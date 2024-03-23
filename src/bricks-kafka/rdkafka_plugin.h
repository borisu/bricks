#pragma once
#include "bricks.h"
#include "rdkafka_common.h"

namespace bricks {

	BRICKSKAFKA_API publisher_plugin_t*
		create_kafka_publisher();

	BRICKSKAFKA_API void
		destroy_kafka_publisher(publisher_plugin_t* service);

	BRICKSKAFKA_API subscriber_plugin_t*
		create_kafka_subscriber();

	BRICKSKAFKA_API void
		destroy_kafka_subscriber(subscriber_plugin_t* service);
}

