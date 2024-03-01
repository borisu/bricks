#pragma once
#include "bricks_api.h"

#ifdef __cplusplus
extern "C" {
#endif

/* logs */
typedef BRICKS_API void (*logger_t)
(const char*);

void BRICKS_API
brick_set_logger(logger_t);


#ifdef __cplusplus
}
#endif


