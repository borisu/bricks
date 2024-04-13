#pragma once
#include "rdkafka_common.h"
#include "queue.h"

using namespace std;

namespace bricks::plugins{

	class kafka_service_t 
	{
	public:

		kafka_service_t();

		virtual ~kafka_service_t();

	protected:

		virtual bricks_error_code_e init_conf(rd_kafka_conf_t* conf, const xtree_t* options);

		virtual void rd_poll_loop();

		virtual bricks_error_code_e rd_poll(int milliseconds, bool last_call) = 0;

		virtual bricks_error_code_e start_rd_poll_loop();

		virtual void stop_rd_poll_loop();

		static void rd_log(const rd_kafka_t* rk, int level, const char* fac, const char* buf);

		bool initiated = false;

		bool started = false;

		atomic<bool> shutdown = false;
		
		cb_queue_t* cb_queue = nullptr;

		thread *rd_poll_thread = nullptr;

		string name;

	};
}

