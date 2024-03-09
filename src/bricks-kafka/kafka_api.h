#pragma once
#include "bricks_api.h"
#include "services_api.h"

#ifdef BRICKSKAFKA_EXPORTS
	#define BRICKSKAFKA_API __declspec(dllexport)
#else
	#define BRICKSKAFKA_API __declspec(dllimport)
#endif


BRICKSKAFKA_API service_t*
service_kafka_create();

BRICKSKAFKA_API void
service_kafka_destroy(service_t *service);

