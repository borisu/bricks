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
	BRICKS_ALARM	= 3,
	BRICKS_LOG_OFF	= 4,
};


/* logs */
typedef  void (*logger_t)
(bricks_debug_level_e, const char* log_str);

BRICKS_API void
brick_set_logger(logger_t);

BRICKS_API void
brick_set_log_level(bricks_debug_level_e);

BRICKS_API bricks_debug_level_e
brick_get_log_level();

BRICKS_API void
log1(bricks_debug_level_e, const char* format, ...);

//https://stackoverflow.com/questions/7775991/how-to-get-hexdump-of-a-structure-data
BRICKS_API void 
hex_dump1(
	bricks_debug_level_e log_level,
	const char* desc,
	const void* addr,
	const int len,
	int perLine);

#ifdef __cplusplus
}
#endif



