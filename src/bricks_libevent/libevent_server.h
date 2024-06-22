#pragma once

namespace bricks::plugins
{
	class libevent_server_t :
		public server_plugin_t
	{
	public:

		libevent_server_t();

		virtual bricks_error_code_e init(cb_queue_t* queue, request_cb_t request, const xtree_t* options) override;

		virtual void release() override { delete this; };

		virtual ~libevent_server_t();

		virtual bool check_capability(plugin_capabilities_e) override;

		virtual void set_meta_cb(meta_cb_t) override {};

	
	private:

		virtual void destroy();

		static void do_term(evutil_socket_t sig, short events, void* arg);

		static void do_handle_request(struct evhttp_request* req, void* arg);

		void do_handle_response(struct evhttp_request* req, bricks_error_code_e, const char*, size_t, xtree_t*);
	
		void libevent_poll_loop();

		string name;

		std::recursive_mutex mtx;

		meta_cb_t meta_cb;

		bool initiated = false;

		bool destroyed = false;

		event_config* cfg = nullptr;

		event_base* base = nullptr;

		evhttp* http = nullptr;

		evhttp_bound_socket* handle = nullptr;

		thread* libevent_poll_thread = nullptr;

		atomic<bool> shutdown = false;

		request_cb_t request;

		event* term = nullptr;

		cb_queue_t* queue = nullptr;

	};

}


