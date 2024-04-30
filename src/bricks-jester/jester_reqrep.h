#pragma once
#include "jester.h"
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

		virtual bricks_error_code_e issue_request(server_proxy_cb_t proxy, const char* data, size_t size);

		virtual void release()  override { delete this; };

	protected:

		jester_server_t* server = nullptr;

	};

	class jester_server_t : public server_plugin_t
	{
	public:

		jester_server_t(jester_server_ctx_t* ctx);

		virtual bricks_error_code_e init(cb_queue_t* queue, const xtree_t* ) override;

		virtual bricks_error_code_e register_request_handler(server_request_cb_t request_cb, const xtree_t* options) override;

		virtual bricks_error_code_e start() override;

		virtual bricks_error_code_e issue_request(server_proxy_cb_t proxy, const char* data, size_t size) ;

		virtual bricks_error_code_e server_response_proxy(server_proxy_cb_t proxy, bricks_error_code_e, const char*, size_t, xtree_t*);

		virtual void release()  override { delete this; };

	protected:

		jester_server_ctx_t* ctx = nullptr;

		cb_queue_t* queue = nullptr;

		server_request_cb_t request_cb;
		
	};

	class jester_client_t : public client_plugin_t
	{
	public:

		jester_client_t(jester_server_ctx_t* ctx);

		virtual bricks_error_code_e init(cb_queue_t* queue, const xtree_t* options ) override;

		virtual bricks_error_code_e request(const char* , size_t , client_response_cb_t, const xtree_t* options ) override;

		virtual bricks_error_code_e start() override;

		virtual bricks_error_code_e client_response_proxy(client_response_cb_t response_cb, bricks_error_code_e, const char*, size_t, xtree_t*);

		virtual void release()  override { delete this; };

	protected:

		jester_server_ctx_t* ctx = nullptr;

		cb_queue_t* queue = nullptr;

	};
}

