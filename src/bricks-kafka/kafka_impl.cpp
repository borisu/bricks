#pragma once
#include "pch.h"
#include "bricks_api.h"
#include "logger_api.h"
#include "kafka_api.h"
#include "xtree_api.h"
#include "services_api.h"
#include "librdkafka/rdkafka.h"
#include "kafka_service.h"


BRICKSKAFKA_API service_t*
service_kafka_create()
{
	return new kafka_subscriber_t();
}

BRICKSKAFKA_API void
service_kafka_destroy(service_t* service)
{
	delete service;
}
