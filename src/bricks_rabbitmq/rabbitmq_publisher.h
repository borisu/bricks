#pragma once
#include "queue.h"
#include "rabbitmq_base.h"

using namespace std;
using namespace bricks;



namespace bricks::plugins {

	class rabbitmq_publisher_t : 
		public rabbitmq_base_t,
		public publisher_plugin_t
	{
	public:

		rabbitmq_publisher_t();

		virtual bricks_error_code_e init(cb_queue_t* queue, const xtree_t* options) override;

		virtual bricks_error_code_e describe_topic(const string& topic, const xtree_t* options) override;

		virtual bricks_error_code_e publish(const string& topic, const char*, size_t, const xtree_t* options)  override;

		virtual void set_meta_cb(meta_cb_t) override;

		virtual bool check_capability(plugin_capabilities_e) override;

		virtual void release() override { delete this; };

		virtual ~rabbitmq_publisher_t();

		void destroy();

	protected:

		std::recursive_mutex mtx;

		meta_cb_t meta_cb;

		bool initiated = false;

		bool destroyed = false;

	};
}

