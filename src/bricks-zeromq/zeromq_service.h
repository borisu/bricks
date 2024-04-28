#pragma once

using namespace std;
using namespace bricks;

namespace bricks::plugins
{
	class zeromq_service_t
	{
	protected:

		zeromq_service_t();

		virtual ~zeromq_service_t();

		virtual void zmq_poll_loop();

		virtual bricks_error_code_e do_zmq_poll(int milliseconds, bool last_call) = 0;

		virtual bricks_error_code_e start_zmq_poll_loop();

		void stop_zmq_poll_loop();

		bool initiated = false;

		bool started = false;

		atomic<bool> shutdown = false;

		thread* zmq_poll_thread = nullptr;

	};
}



