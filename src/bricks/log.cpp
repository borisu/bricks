#include "pch.h"
#include "logger_api.h"
#include "bricks_api.h"

#define MAX_LOG_LEN 2048

thread_local char log_buf[MAX_LOG_LEN];

void console_logger(const char* l)
{
	printf("%s", l);
}

logger_t ext_logger = nullptr;

void log(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	vsprintf_s(log_buf, format, args);
	ext_logger == nullptr ? console_logger(log_buf) : ext_logger(log_buf);

	va_end(args);
}

