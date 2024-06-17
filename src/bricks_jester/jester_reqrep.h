#pragma once
#include "jester_plugin.h"
#include <map>

using namespace std;
using namespace bricks;

namespace bricks::plugins {

	class jester_server_t;
	class jester_client_t;

	class jester_server_ctx_t : public brick_t
	{
	public:

		jester_server_ctx_t();

		void set_server(jester_server_t* server);

		virtual bricks_error_code_e issue_request(response_proxy_cb_t proxy, const char* data, size_t size);

		virtual void release()  override { delete this; };

		virtual void set_meta_cb(meta_cb_t) {};

		std::recursive_mutex mtx;

	protected:

		jester_server_t* server = nullptr;

	};

	class jester_server_t : public server_plugin_t
	{
	public:

		jester_server_t(jester_server_ctx_t* ctx);

		virtual bricks_error_code_e init(cb_queue_t* queue, request_cb_t request_cb, const xtree_t* ) override;

		virtual bricks_error_code_e issue_request(response_proxy_cb_t proxy, const char* data, size_t size) ;

		virtual bricks_error_code_e response_proxy(response_proxy_cb_t proxy, bricks_error_code_e, const char*, size_t, xtree_t*);

		virtual bool check_capability(plugin_capabilities_e) override;

		virtual void release()  override { delete this; };

		virtual void set_meta_cb(meta_cb_t) override {};

	protected:

		jester_server_ctx_t* ctx = nullptr;

		cb_queue_t* queue = nullptr;

		request_cb_t request_cb;

		bool initiated = false;

		bool started = false;
		
	};

	class jester_client_t : public client_plugin_t
	{
	public:

		jester_client_t(jester_server_ctx_t* ctx);

		virtual bricks_error_code_e init(cb_queue_t* queue, int timeout_ms, const xtree_t* options = nullptr) override;

		virtual bricks_error_code_e issue_request(const char* , size_t , response_cb_t, const xtree_t* options ) override;

		virtual bricks_error_code_e client_response_proxy(response_cb_t response_cb, bricks_error_code_e, const char*, size_t, xtree_t*);

		virtual bool check_capability(plugin_capabilities_e) override;

		virtual void release()  override { delete this; };

		virtual void set_meta_cb(meta_cb_t) override {};

	protected:

		jester_server_ctx_t* ctx = nullptr;

		cb_queue_t* queue = nullptr;

		int timeout_ms = BRICKS_DEFAULT_CLIENT_TIMEOUT;

		bool initiated = false;

		bool started = false;

	};
}

