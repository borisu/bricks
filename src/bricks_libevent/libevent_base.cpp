#include "pch.h"
#include "libevent_base.h"
#include "libevent_plugin.h"

using namespace std;
using namespace bricks;
using namespace bricks::plugins;

static void log_cb(int severity, const char* msg)
{

	bricks_debug_level_e l = BRICKS_LOG_OFF;

	switch (severity)
	{
	case EVENT_LOG_DEBUG:
	{
		l = BRICKS_DEBUG;
		break;
	}
	case EVENT_LOG_MSG:
	{
		l = BRICKS_INFO;
		break;
	}
	case EVENT_LOG_WARN:
	case EVENT_LOG_ERR:
	default:
	{
		l = BRICKS_ALARM;
	}
	}

	log1(BRICKS_ALARM, "(libevent) %%%%%% %s.", msg);
}

bricks_error_code_e
bricks::plugins::libevent_init(bool debug)
{
#ifdef _WIN32
	{
		WORD wVersionRequested;
		WSADATA wsaData;
		wVersionRequested = MAKEWORD(2, 2);
		if (WSAStartup(wVersionRequested, &wsaData) != 0)
		{
			log1(BRICKS_ALARM, "(libevent_init) %%%%%% Couldn't initiate sockets (WSAStartup).");
			return BRICKS_FAILURE_GENERIC;
		}
	}
#endif

	if (debug)
		event_enable_debug_logging(EVENT_DBG_ALL);

	event_set_log_callback(log_cb);

#ifdef EVTHREAD_USE_WINDOWS_THREADS_IMPLEMENTED
	evthread_use_windows_threads();
#elif EVTHREAD_USE_PTHREADS_IMPLEMENTED
	int evthread_use_pthreads(void);
#else
	#error no multithreading library defined
#endif

	return BRICKS_SUCCESS;
}

void
bricks::plugins::libevent_destroy()
{
	libevent_global_shutdown();
}