#pragma once
#include "bricks_api.h"

#ifdef __cplusplus
extern "C" {
#endif

enum bricks_debug_level_e
{
	BRICKS_TRACE	= 1,
	BRICKS_DEBUG	= 2,
	BRICKS_INFO		= 3,
	BRICKS_NONE		= 4,
};


/* logs */
typedef  void (*logger_t)
(bricks_debug_level_e, const char* log_str);

BRICKS_API void
brick_set_logger(logger_t);

BRICKS_API void
brick_set_log_level(bricks_debug_level_e);

BRICKS_API void
log1(bricks_debug_level_e, const char* format, ...);


#ifdef __cplusplus
}
#endif



