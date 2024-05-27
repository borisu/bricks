#pragma once
#include "timer.h"
#include "utils.h"

namespace bricks
{ 
	class pubsub_client_t : public client_plugin_t
	{
	public:

		pubsub_client_t(
			publisher_plugin_t* publisher, 
			subscriber_plugin_t *subscriber,
			timer_t *timer,
			const char *request_topic_prefix,
			const char *reponse_topic_prefix);

		virtual bricks_error_code_e init(cb_queue_t* queue, int timeout_ms, const xtree_t* options ) override;

		virtual bricks_error_code_e issue_request(const char*, size_t, response_cb_t, const xtree_t* options ) override;
		
		virtual bricks_error_code_e start() override;

		virtual void release()  override { delete this; };

		virtual bool check_capability(plugin_capabilities_e) override;

	protected:	

		virtual void topic_cb(const string&, buffer_t*, xtree_t*);

		virtual void timeout_cb(const string&);

		publisher_plugin_t* publisher = nullptr;

		subscriber_plugin_t *subscriber = nullptr;

		cb_queue_t* queue = nullptr;

		string request_topic_prefix;

		string response_topic_prefix;

		struct request_ctx
		{
			response_cb_t request_cb;
			int handle;
		};

		map<string, request_ctx> ctxs;

		atomic<bool> initiated = false;

		atomic<bool> started = false;

		timer_t *timer = nullptr;

		int timeout_ms = BRICKS_DEFAULT_CLIENT_TIMEOUT ;
		
	};
}

