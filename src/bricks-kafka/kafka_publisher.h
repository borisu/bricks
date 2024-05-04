#pragma once
#include "kafka_service.h"

using namespace std;

namespace bricks::plugins
{

	class kafka_publisher_t :
		public kafka_service_t,
		public publisher_plugin_t
	{
	public:

		kafka_publisher_t();

		virtual bricks_error_code_e init(cb_queue_t* queue, const xtree_t* options) override;

		virtual bricks_error_code_e register_topic(const string& topic, const xtree_t* options) override;

		virtual bricks_error_code_e publish(const string& topic, const char*, size_t, const xtree_t* options)  override;

		virtual bricks_error_code_e rd_poll(int milliseconds, bool last_call) override;

		virtual bricks_error_code_e start()  override;

		virtual void release() override { delete this; };

		virtual ~kafka_publisher_t();

	private:

		virtual void destroy();

		rd_kafka_conf_t* rd_conf_h = nullptr;

		rd_kafka_t* rd_producer_h = nullptr;

		map<string, rd_kafka_topic_t*> rd_topics;

	};

}



