#include "pch.h"
#include "logger_api.h"
#include "bricks_api.h"

#define MAX_LOG_LEN 2048

thread_local char log_buf[MAX_LOG_LEN];

void console_logger(bricks_debug_level_e log_level, const char* l)
{
	printf("%s", l);
}

logger_t ext_logger = nullptr;

std::atomic<bricks_debug_level_e> curr_log_level = BRICKS_TRACE;

#ifdef __cplusplus
extern "C" {
#endif

void log1(bricks_debug_level_e log_level, const char* format, ...)
{
	if (log_level < curr_log_level)
		return;

	va_list args;
	va_start(args, format);
	vsprintf_s(log_buf, format, args);
	ext_logger == nullptr ? console_logger(log_level, log_buf) : ext_logger(log_level, log_buf);

	va_end(args);
}

void brick_set_logger(logger_t l)
{
	ext_logger = l;
}

void brick_set_log_level(bricks_debug_level_e l)
{
	curr_log_level = l;
}



#ifdef __cplusplus
}
#endif