#pragma once
#include "bricks.h"
#include "kafka_service.h"


using namespace std;

namespace bricks::plugins {

	class kafka_subscriber_t :
		public kafka_service_t,
		public subscriber_plugin_t
	{
	public:

		kafka_subscriber_t();

		virtual bricks_error_code_e init(cb_queue_t* queue, msg_cb_t msg_cb, const xtree_t* options) override;

		virtual bricks_error_code_e register_topic(const string& topic, const xtree_t* options) override;

		virtual bricks_error_code_e start() override;

		virtual bricks_error_code_e rd_poll(int milliseconds, bool last_call) override;

		virtual void name(const char*) override;

		virtual const char* name() const override;

		virtual void release() override { delete this; };

		virtual ~kafka_subscriber_t();

	private:

		virtual void destroy();

		rd_kafka_conf_t* rd_conf_h = nullptr;

		rd_kafka_t* rd_kafka_h = nullptr;

		rd_kafka_topic_partition_list_t* rd_part_list_h = nullptr;

		msg_cb_t msg_cb;

		void* opaque = nullptr;

		

	};

}



