#pragma once
#include "bricks_api.h"
#include "services_api.h"

#ifdef BRICKSKAFKA_EXPORTS
	#define BRICKSKAFKA_API __declspec(dllexport)
#else
	#define BRICKSKAFKA_API __declspec(dllimport)
#endif


BRICKSKAFKA_API publisher_service_t*
create_kafka_publisher();

BRICKSKAFKA_API void
destroy_kafka_publisher(publisher_service_t *service);

BRICKSKAFKA_API subscriber_service_t*
create_kafka_subscriber();

BRICKSKAFKA_API void
destroy_kafka_subscriber(subscriber_service_t* service);

