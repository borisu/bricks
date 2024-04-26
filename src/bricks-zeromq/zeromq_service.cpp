#include "pch.h"
#include "zeromq_service.h"

using namespace bricks;
using namespace bricks::plugins;

zeromq_service_t::zeromq_service_t()
{
	
}

bricks_error_code_e
zeromq_service_t::start_zmq_poll_loop()
{
	ASSERT_NOT_STARTED;

	zmq_poll_thread = new thread(&zeromq_service_t::zmq_poll_loop, this);

	started = true;

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


