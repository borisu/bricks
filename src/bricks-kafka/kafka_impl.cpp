#pragma once
#include "pch.h"
#include "bricks_api.h"
#include "kafka_api.h"

#ifdef __cplusplus
extern "C" {
#endif

bricks_handle_t
bricks_kafka_init(bricks_handle_t config_xt) { return BRICKS_SUCCESS; };

void
bricks_kafka_destroy() {  };

bricks_error_code_e
bricks_kafka_register_service(char* svc) { return 0; };

bricks_error_code_e
bricks_kafka_unregister_service(char* svc) { return BRICKS_SUCCESS; };

#ifdef __cplusplus
}
#endif
