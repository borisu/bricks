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

		void libevent_poll_loop();

		static void do_term(evutil_socket_t sig, short events, void* arg);

		static void timeout_cb(evutil_socket_t fd, short what, void* arg);

		static void http_request_done(evhttp_request* conn, void* arg);

		void destroy();

		meta_cb_t meta_cb;

		thread* libevent_poll_thread = nullptr;

		atomic<bool> shutdown = false;

		string name;

		std::mutex mtx;

		cb_queue_t* queue = nullptr;

		bool initiated = false;

		bool destroyed = false;

		event_config* cfg = nullptr;

		event_base* base = nullptr;

		int timeout_ms = 10000;

		event* term = nullptr;

		int counter = 0;

		struct req_ctx
		{
			int counter; 

			libevent_client_t* This;

			evhttp_connection* conn;

			evhttp_request* req;

			event* timeout_event;

			response_cb_t response_cb;

		};

		map<int, req_ctx*> ctxs;

	};

}


