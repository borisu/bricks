#include "pch.h"
#include "zeromq_service.h"

using namespace bricks;
using namespace bricks::plugins;

zeromq_service_t::zeromq_service_t()
{
	
}

void zeromq_service_t::monitor_callback(void* arg, int event, int value, void* socket)
{
    const char* event_str = NULL;
    switch (event) {
    case ZMQ_EVENT_CONNECTED:
        event_str = "CONNECTED";
        break;
    case ZMQ_EVENT_DISCONNECTED:
        event_str = "DISCONNECTED";
        break;
    case ZMQ_EVENT_MONITOR_STOPPED:
        event_str = "MONITOR_STOPPED";
        break;
    case ZMQ_EVENT_ALL:
        event_str = "ALL";
        break;
    default:
        event_str = "UNKNOWN";
    }
    printf("Event: %s, value: %d\n", event_str, value);
}


bricks_error_code_e
zeromq_service_t::start_zmq_poll_loop()
{
	ASSERT_NOT_STARTED;

	zmq_poll_thread = new thread(&zeromq_service_t::zmq_poll_loop, this);

	return BRICKS_SUCCESS;
}

void
zeromq_service_t::stop_zmq_poll_loop()
{
	if (zmq_poll_thread)
	{
		shutdown = true;
		zmq_poll_thread->join();
		zmq_poll_thread = nullptr;
	}
}

#define ZMQ_POLL_TIMEOUT 500

void
zeromq_service_t::zmq_poll_loop()
{
	while (shutdown != true)
	{
		auto err = do_zmq_poll(ZMQ_POLL_TIMEOUT, false);
		switch (err)
		{
		case BRICKS_TIMEOUT:
		case BRICKS_SUCCESS:
			continue;
		default:
			break;

		}
	}

	do_zmq_poll(ZMQ_POLL_TIMEOUT, true);

}

zeromq_service_t::~zeromq_service_t()
{
	stop_zmq_poll_loop();
}


