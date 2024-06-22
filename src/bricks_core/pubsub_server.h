#pragma once
#include "timer.h"

namespace bricks
{
	class pubsusb_server_t : public server_plugin_t
	{
	public:

		pubsusb_server_t(
			publisher_plugin_t* publisher,
			subscriber_plugin_t* subscriber,
			const char* request_topic_prefix,
			const char* reponse_topic_prefix,
			const char* error_topic_prefix);

		virtual bricks_error_code_e init(cb_queue_t* queue, request_cb_t request, const xtree_t* options) override;

		virtual void topic_cb(const string&, buffer_t*, xtree_t*);

		virtual void response_proxy_cb(const string& topic, bricks_error_code_e, const char*, size_t, xtree_t*);
		
		virtual void release()  override { delete this; };

		virtual bool check_capability(plugin_capabilities_e) override;

		virtual void set_meta_cb(meta_cb_t) { }

	protected:

		publisher_plugin_t* publisher = nullptr;

		subscriber_plugin_t* subscriber = nullptr;

		cb_queue_t* queue = nullptr;

		string request_topic_prefix;

		string response_topic_prefix;

		string error_topic_prefix;

		map<string, response_cb_t> clients;

		request_cb_t request_cb;

		std::mutex mtx;

		bool initiated = false;

		bool destroyed = false;

	};
}

