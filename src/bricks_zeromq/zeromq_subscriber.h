#pragma once
#include "zeromq_service.h"

using namespace std;

namespace bricks::plugins
{

	class zeromq_subscriber_t :
		public zeromq_service_t,
		public subscriber_plugin_t
	{
	public:

		zeromq_subscriber_t();

		virtual bricks_error_code_e init(cb_queue_t* queue, topic_cb_t msg_cb, const xtree_t* options)  override;

		virtual bricks_error_code_e subscribe(const string& topic, const xtree_t* options) override;

		virtual ~zeromq_subscriber_t();

		virtual bool check_capability(plugin_capabilities_e) override;

		virtual bricks_error_code_e unsubscribe(const std::string&, const xtree_t* options) override;

		virtual void release() override { delete this; };

		virtual void set_meta_cb(meta_cb_t meta_cb) override;

		virtual void do_destroy() override;

	protected:

		virtual bricks_error_code_e do_zmq_poll(int milliseconds, bool last_call) override;

	private:

		void destroy();

		topic_cb_t msg_cb;

		void* context = nullptr;

		void* subscriber = nullptr;

		zmq_pollitem_t items[1] = { 0 };

		string url;

		meta_cb_t meta_cb;

	};
}

