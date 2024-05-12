#pragma once
#include "pch.h"
#include "bricks.h"
#include "librdkafka/rdkafka.h"
#include "kafka_service.h"
#include "kafka_publisher.h"
#include "kafka_subscriber.h"
#include "rdkafka_plugin.h"


using namespace bricks;
using namespace bricks::plugins;

publisher_plugin_t*
bricks::plugins::create_kafka_publisher()
{
	return new kafka_publisher_t();
}



subscriber_plugin_t*
bricks::plugins::create_kafka_subscriber()
{
	return new kafka_subscriber_t();
}



