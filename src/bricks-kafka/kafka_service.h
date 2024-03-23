#pragma once
#include "locking_queue.h"
#include "rdkafka_common.h"

namespace bricks {

	class kafka_service_t 
	{
	public:

		kafka_service_t();

		virtual bricks_error_code_e poll(int milliseconds);

		virtual ~kafka_service_t();

	protected:

		virtual bricks_error_code_e init_conf(rd_kafka_conf_t* conf, const xtree_t* options);

		virtual void rd_poll_loop();

		virtual bricks_error_code_e rd_poll(int milliseconds) = 0;

		virtual bricks_error_code_e start_rd_poll_loop();

		virtual void stop_rd_poll_loop();

		static void	rd_log(const rd_kafka_t* rk, int level, const char* fac, const char* buf);

		bool initiated = false;

		bool started = false;

		atomic<bool> shutdown = false;
		
		LockingQueue<bound_callback_t> cb_queue;

		thread *rd_poll_thread = nullptr;

	};
}

