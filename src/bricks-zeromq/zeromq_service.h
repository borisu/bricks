#pragma once

#define ZMQ_ASSERT_INITIATED if (!this->initiated) return BRICKS_INVALID_STATE
#define ZMQ_ASSERT_NOT_INITIATED if (this->initiated) return BRICKS_INVALID_STATE

#define ZMQ_ASSERT_STARTED if (!this->started) return BRICKS_INVALID_STATE
#define ZMQ_ASSERT_NOT_STARTED if (this->started) return BRICKS_INVALID_STATE


class zeromq_service_t
{
protected:

	zeromq_service_t();

	static void monitor_callback(void* arg, int event, int value, void* socket);

	bool initiated = false;

	bool started = false;

	request_cb_t req_cb = nullptr;

};

