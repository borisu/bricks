#pragma once
#include "pch.h"
#include "bricks_api.h"
#include "logger_api.h"
#include "kafka_api.h"
#include "xtree_api.h"
#include "services_api.h"
#include "librdkafka/rdkafka.h"
#include "kafka_service.h"
#include "kafka_publisher.h"
#include "kafka_subscriber.h"



publisher_service_t*
create_kafka_publisher()
{
	return new kafka_publisher_t();
}

void
destroy_kafka_publisher(publisher_service_t* service)
{
	delete service;
}

subscriber_service_t*
create_kafka_subscriber()
{
	return new kafka_subscriber_t();
}

void
destroy_kafka_subscriber(subscriber_service_t* service)
{
	delete service;
}


