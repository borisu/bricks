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

bricks_debug_level_e
brick_get_log_level()
{
    return curr_log_level;
}

#define MAX_BYTES_PER_LINE 64
#define MAX_LINE_LEN MAX_BYTES_PER_LINE*4+1

void hex_dump(
    bricks_debug_level_e log_level,
    const char* desc,
    const void* addr,
    const int len,
    int perLine
) {
    if (log_level < curr_log_level)
        return;

    // Silently ignore silly per-line values.

    if (perLine < 4 || perLine > MAX_BYTES_PER_LINE) perLine = 16;

    int i;
    unsigned char buff[MAX_LINE_LEN] = { '\0' };

    unsigned char line_buff[MAX_LINE_LEN] = { '\0' };

    int line_offset = 0;

    const unsigned char* pc = (const unsigned char*)addr;

    // Output description if given.

    if (desc != NULL) log1(log_level, "%s:\n", desc);

    // Length checks.

    if (len == 0) {
        log1(log_level, "  ZERO LENGTH\n");
        return;
    }
    if (len < 0) {
        log1(log_level, "  NEGATIVE LENGTH: %d\n", len);
        return;
    }

    // Process every byte in the data.

    for (i = 0; i < len; i++) {
        // Multiple of perLine means new or first line (with line offset).

        if ((i % perLine) == 0) {
            // Only print previous-line ASCII buffer for lines beyond first.

            if (i != 0) {
                log1(log_level,"%s  %s\n", line_buff, buff);
                line_offset = 0;
            }

            // Output the offset of current line.
            line_offset += snprintf((char*)(line_buff + line_offset), MAX_LINE_LEN - line_offset, "  %04x ", i);
            
        }

        // Now the hex code for the specific character.
        line_offset += snprintf((char*)(line_buff + line_offset), MAX_LINE_LEN - line_offset, " %02x", pc[i]);

        // And buffer a printable ASCII character for later.

        if ((pc[i] < 0x20) || (pc[i] > 0x7e)) // isprint() may be better.
            buff[i % perLine] = '.';
        else
            buff[i % perLine] = pc[i];
        buff[(i % perLine) + 1] = '\0';
    }

    // Pad out last line if not exactly perLine characters.

    while ((i % perLine) != 0) {
        line_offset += snprintf((char*)(line_buff + line_offset), MAX_LINE_LEN - line_offset, "   ");
        i++;
    }

    // And print the final ASCII buffer.
    log1(log_level, "%s  %s\n", line_buff, buff);
    
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