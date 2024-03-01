#pragma once
#include "bricks_api.h"


#ifdef __cplusplus
extern "C" {
#endif

enum bricks_service_type_e
{
	BRICKS_NONE = 0x0,
	BRICKS_PUBLISHER = 0x1,
	BRICKS_SUBSCRIBER = 0x1 << 1
};

BRICKS_API bricks_error_code_e
bricks_register_service_provider(const char* name, bricks_handle_t h);


BRICKS_API bricks_error_code_e
bricks_service_publish(const char* name, bricks_handle_t xt);


#ifdef __cplusplus
}
#endif
