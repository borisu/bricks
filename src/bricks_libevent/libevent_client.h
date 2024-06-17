#pragma once

namespace bricks::plugins
{
	class libevent_client_t : public client_plugin_t
	{
	public:

		libevent_client_t();
		
		virtual bricks_error_code_e init(cb_queue_t* queue, int timeout_ms, const xtree_t* options ) override;

		virtual bricks_error_code_e issue_request(const char*, size_t, response_cb_t, const xtree_t* options ) override;

		virtual void set_meta_cb(meta_cb_t) override;

		virtual bool check_capability(plugin_capabilities_e) override;

		virtual void release() override { delete this; };

		virtual ~libevent_client_t();

	protected:

		meta_cb_t meta_cb;
	};

}


