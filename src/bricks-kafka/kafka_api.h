#pragma once
#include "bricks_api.h"
#include "services_api.h"

#ifdef BRICKSKAFKA_EXPORTS
	#define BRICKSKAFKA_API __declspec(dllexport)
#else
	#define BRICKSKAFKA_API __declspec(dllimport)
#endif

#ifdef __cplusplus
extern "C" {
#endif

BRICKSKAFKA_API bricks_handle_t
bricks_kafka_init(bricks_handle_t kafka_config_h, msg_delivered_cb_t kafka_msg_delivered_cb);

BRICKSKAFKA_API void
bricks_kafka_destroy(bricks_handle_t kafka_h);

BRICKSKAFKA_API bricks_error_code_e
bricks_kafka_register_topic(bricks_handle_t kafka_h, const char* topic);

BRICKSKAFKA_API bricks_error_code_e
bricks_kafka_unregister_topic(bricks_handle_t kafka_h, const char* topic);

#ifdef __cplusplus
}
#endif