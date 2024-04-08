#pragma once
#include "common.h"

namespace bricks {

	enum bricks_debug_level_e
	{
		BRICKS_TRACE,
		BRICKS_DEBUG,
		BRICKS_INFO,
		BRICKS_ALARM,
		BRICKS_FATAL,
		BRICKS_LOG_OFF
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
}
	


