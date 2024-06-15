#pragma once
#include "queue.h"
#include "rabbitmq_base.h"

using namespace std;
using namespace bricks;


namespace bricks::plugins {

	class rabbitmq_subscriber_t : 
		public rabbitmq_base_t,
		public subscriber_plugin_t
	{
	public:

		rabbitmq_subscriber_t();

		virtual bricks_error_code_e init(cb_queue_t* queue, topic_cb_t msg_cb, const xtree_t* options ) override;

		virtual bricks_error_code_e subscribe(const string& topic, const xtree_t* options ) override;

		virtual bricks_error_code_e unsubscribe(const string& topic, const xtree_t* options ) override;

		virtual bricks_error_code_e unsubscribe(const xtree_t* options ) override;

		virtual bricks_error_code_e start()  override;

		virtual bool check_capability(plugin_capabilities_e) override;

		virtual void release() override { delete this; };

		virtual ~rabbitmq_subscriber_t();

		void destroy();

	protected:

		virtual void rmq_poll_loop();

		virtual void on_topic(std::string pattern, std::string channel, std::string msg);

		std::recursive_mutex mtx;

		topic_cb_t msg_cb;

		thread* rmq_poll_thread = nullptr;

		bool initiated = false;

		bool started = false;

		string default_queue_name;

	};
}

