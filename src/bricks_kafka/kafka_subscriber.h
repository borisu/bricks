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

		virtual bricks_error_code_e init(cb_queue_t* queue, topic_cb_t msg_cb, const xtree_t* options) override;

		virtual bricks_error_code_e subscribe(const string& topic, const xtree_t* options) override;

		virtual bricks_error_code_e unsubscribe(const string& topic, const xtree_t* options ) override;

		virtual bricks_error_code_e unsubscribe(const xtree_t* options ) override;

		virtual bricks_error_code_e start() override;

		virtual bool check_capability(plugin_capabilities_e) override;

		virtual void release() override { delete this; };

		virtual ~kafka_subscriber_t();

	private:

		virtual void destroy();

		virtual bricks_error_code_e rd_poll(int milliseconds, bool last_call) override;

		rd_kafka_conf_t* rd_conf_h = nullptr;

		rd_kafka_t* rd_kafka_h = nullptr;

		rd_kafka_topic_partition_list_t* rd_part_list_h = nullptr;

		topic_cb_t msg_cb;

	};

}



