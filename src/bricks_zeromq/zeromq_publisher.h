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

		virtual bricks_error_code_e start() override ;

		virtual void release() override { delete this; };

		virtual bool check_capability(plugin_capabilities_e) override;

		virtual ~zeromq_publisher_t();

	private:

		void destroy();
	
		void* context = nullptr;

		void* publisher = nullptr;

		string url;

		cb_queue_t* cb_queue = nullptr;

		string bname;

	};


}

