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

		virtual bricks_error_code_e init(cb_queue_t* queue, msg_cb_t msg_cb, const xtree_t* options)  override;

		virtual bricks_error_code_e register_topic(const string& topic, const xtree_t* options) override;

		virtual bricks_error_code_e start() override;

		virtual ~zeromq_subscriber_t();

		virtual void name(const char*) override;

		virtual const char* name() const override;

		virtual void release() override { delete this; };

	protected:

		virtual bricks_error_code_e do_zmq_poll(int milliseconds, bool last_call) override;

	private:

		void destroy();

		msg_cb_t msg_cb;

		void* context = nullptr;

		void* subscriber = nullptr;

		void* opaque = nullptr;

		cb_queue_t* cb_queue = nullptr;

		zmq_pollitem_t items[1] = { 0 };

		string url;

		string bname;

	};
}

