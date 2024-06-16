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

		virtual void set_meta_cb(meta_cb_t) override;

		virtual bricks_error_code_e start()  override;

		virtual bool check_capability(plugin_capabilities_e) override;

		virtual void release() override { delete this; };

		virtual ~rabbitmq_subscriber_t();

	protected:

		virtual void do_subscribe(std::promise<bricks_error_code_e> &p, const string& topic, const xtree_t* options);

		virtual void do_unsubscribe1(std::promise<bricks_error_code_e> &p, const string& topic, const xtree_t* options);

		virtual void do_unsubscribe2(std::promise<bricks_error_code_e> &p, const xtree_t* options);

		void destroy();

		void destroy_s();

		virtual void rmq_poll_loop();

		std::mutex mtx;

		topic_cb_t msg_cb;

		meta_cb_t meta_cb;

		thread* rmq_poll_thread = nullptr;

		bool initiated = false;

		bool destroyed = false;

		string default_queue_name;

		atomic<bool> shutdown = false;

		cb_queue_t* rmq_queue = nullptr;

		selector_t* rmq_selector = nullptr;


	};
}

