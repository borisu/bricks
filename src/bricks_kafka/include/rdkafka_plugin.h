#pragma once
#include "bricks.h"

#ifdef BRICKSKAFKA_EXPORTS
#define BRICKSKAFKA_API __declspec(dllexport)
#else
#define BRICKSKAFKA_API __declspec(dllimport)
#endif


namespace bricks::plugins {

	BRICKSKAFKA_API publisher_plugin_t*
		create_kafka_publisher();

	BRICKSKAFKA_API subscriber_plugin_t*
		create_kafka_subscriber();

}

