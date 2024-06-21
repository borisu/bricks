#pragma once
#include "zeromq_service.h"


using namespace std;

namespace bricks::plugins
{

	class zeromq_bidi_t :
		public zeromq_service_t,
		public bidi_plugin_t
	{
	public:

		zeromq_bidi_t();

		virtual bricks_error_code_e init(cb_queue_t* queue, event_cb_t event_cb, const xtree_t* options ) override;

		virtual bricks_error_code_e send_event(const char*, size_t, const xtree_t* options ) override;

		virtual void release() override { delete this; };

		virtual bool check_capability(plugin_capabilities_e) override;
		
		virtual ~zeromq_bidi_t();

		virtual void set_meta_cb(meta_cb_t) override {};

	protected:

		void destroy();

		virtual void do_destroy() override;

		virtual bricks_error_code_e do_zmq_poll(int milliseconds, bool last_call) override;

		zmq_pollitem_t items[1] = { 0 };

		void* context = nullptr;

		void* pair = nullptr;

		bool is_server = false;

		event_cb_t event_cb = nullptr;

		string url;

		meta_cb_t meta_cb;

	};
}

