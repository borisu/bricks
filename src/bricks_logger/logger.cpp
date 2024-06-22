#include "pch.h"
#include "logger.h"

#define MAX_LOG_LEN 2048

using namespace std;
using namespace bricks;

thread_local char log_buf[MAX_LOG_LEN];

const char* debug_str[] = { "TRACE", "DEBUG", "INFO", "ALARM", "FATAL", "OFF" };

void console_logger(bricks_debug_level_e log_level, const char* l)
{
	char buffer[20] = { 0 };

	// Get the current time
	time_t curr_time = time(NULL);

	struct tm tm_buf;

#ifdef WIN32
	// Convert it to local time
	localtime_s(&tm_buf , &curr_time);
#else
	localtime_r(&curr_time , &tm_buf);
#endif

	// Format the time into the buffer
	strftime(buffer, 20, "%Y-%m-%d %H:%M:%S", &tm_buf);

	if (log_level < BRICKS_TRACE || log_level > BRICKS_LOG_OFF)
		return;

	printf("%s: [%s] - %s\n", buffer, debug_str[log_level], l);

};

logger_t ext_logger = nullptr;

std::atomic<bricks_debug_level_e> curr_log_level = BRICKS_ALARM;

void
bricks::log1(bricks_debug_level_e log_level, const char* format, ...)
{
	if (log_level < curr_log_level)
		return;

	va_list args;
	va_start(args, format);
	vsnprintf(log_buf, MAX_LOG_LEN,format, args);
	ext_logger == nullptr ? console_logger(log_level, log_buf) : ext_logger(log_level, log_buf);

	va_end(args);
}

bricks_debug_level_e
brick_get_log_level()
{
	return curr_log_level;
}

void brick_set_logger(logger_t l)
{
	ext_logger = l;
}

void brick_set_log_level(bricks_debug_level_e l)
{
	curr_log_level = l;
}


