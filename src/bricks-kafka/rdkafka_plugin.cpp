#pragma once
#include "pch.h"
#include "bricks.h"
#include "librdkafka/rdkafka.h"
#include "kafka_service.h"
#include "kafka_publisher.h"
#include "kafka_subscriber.h"
#include "rdkafka_plugin.h"


using namespace bricks;

publisher_plugin_t*
bricks::create_kafka_publisher()
{
	return new kafka_publisher_t();
}

void
bricks::destroy_kafka_publisher(publisher_plugin_t* service)
{
	delete service;
}

subscriber_plugin_t*
bricks::create_kafka_subscriber()
{
	return new kafka_subscriber_t();
}

void
bricks::destroy_kafka_subscriber(subscriber_plugin_t* service)
{
	delete service;
}


