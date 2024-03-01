#pragma once
#include "bricks_api.h"

#ifdef BRICKSKAFKA_EXPORTS
	#define BRICKSKAFKA_API __declspec(dllexport)
#else
	#define BRICKSKAFKA_API __declspec(dllimport)
#endif

BRICKSKAFKA_API bricks_error_code_e
bricks_kafka_init(bricks_handle_t config_xt);

BRICKSKAFKA_API void
bricks_kafka_destroy();

BRICKSKAFKA_API bricks_handle_t
bricks_kafka_register_service(char* svc);

BRICKSKAFKA_API bricks_error_code_e
bricks_kafka_unregister_service(char* svc);
