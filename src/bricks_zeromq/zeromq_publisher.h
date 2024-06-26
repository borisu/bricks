#pragma once
#include "zeromq_service.h"


using namespace std;

namespace bricks::plugins
{
	class zeromq_publisher_t :
		public zeromq_service_t,
		public publisher_plugin_t
	{
	public:

		zeromq_publisher_t();

		virtual bricks_error_code_e init(cb_queue_t* queue,  const xtree_t* options) override;

		virtual bricks_error_code_e describe_topic(const string& topic, const xtree_t* options) override;

		virtual bricks_error_code_e publish(const string& topic, const char*, size_t, const xtree_t* options)  override;

		virtual bricks_error_code_e do_zmq_poll(int, bool) override { return BRICKS_NOT_SUPPORTED; }

		virtual void release() override { delete this; };

		virtual bool check_capability(plugin_capabilities_e) override;

		virtual ~zeromq_publisher_t();

		virtual void set_meta_cb(meta_cb_t) override ;

	private:

		void destroy();

		virtual void do_destroy() override;
	
		void* context = nullptr;

		void* publisher = nullptr;

		string url;

		meta_cb_t meta_cb;

	};


}

