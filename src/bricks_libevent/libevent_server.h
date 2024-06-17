#pragma once

namespace bricks::plugins
{
	class libevent_server_t :
		public server_plugin_t
	{
	public:

		libevent_server_t();

		virtual bricks_error_code_e init(cb_queue_t* queue, const xtree_t* options) override;

		virtual bricks_error_code_e register_request_cb(request_cb_t request, const xtree_t* options) override;

		virtual void release() override { delete this; };

		virtual ~libevent_server_t();

		virtual bool check_capability(plugin_capabilities_e) override;

	private:

		event_config* cfg = nullptr;

		event_base* base = nullptr;

		evhttp* http = nullptr;

		evhttp_bound_socket* handle = nullptr;
	};

}


