#include "pch.h"
#include "zeromq_service.h"

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