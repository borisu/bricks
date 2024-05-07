#pragma once
#include <functional>
#include "common.h"
#include "xtree.h"
#include "queue.h"
#include "buffer.h"

using namespace std;

namespace bricks {

	class plugin_t : public brick_t {
	public:

		virtual bricks_error_code_e start() = 0;
	};

	class publisher_plugin_t : public plugin_t
	{
	public:

		virtual bricks_error_code_e init(cb_queue_t *queue, const xtree_t* options = nullptr) = 0;

		virtual bricks_error_code_e register_topic(const string& topic, const xtree_t* options = nullptr) = 0;

		virtual bricks_error_code_e publish(const string& topic, const char*, size_t, const xtree_t* options = nullptr) = 0;

	};

	typedef
		function<void(const string&, buffer_t*, xtree_t*)> msg_cb_t;

	class subscriber_plugin_t : public plugin_t
	{
	public:

		virtual bricks_error_code_e init(cb_queue_t *queue, msg_cb_t msg_cb, const xtree_t* options = nullptr) = 0;

		virtual bricks_error_code_e register_topic(const string& topic, const xtree_t* options = nullptr) = 0;

	};

	typedef function <void(bricks_error_code_e, const char*, size_t, xtree_t*)>
		server_proxy_cb_t;

	typedef function<void(server_proxy_cb_t, buffer_t*, xtree_t*)>
		server_request_cb_t;

	class server_plugin_t : public plugin_t
	{
	public:

		virtual bricks_error_code_e init(cb_queue_t *queue, const xtree_t* options = nullptr) = 0;

		virtual bricks_error_code_e register_request_handler(server_request_cb_t request, const xtree_t* options = nullptr) = 0;

	};


	typedef function<void(bricks_error_code_e, buffer_t*, xtree_t*)>
		client_response_cb_t;

	class client_plugin_t : public plugin_t
	{
	public:

		virtual bricks_error_code_e init(cb_queue_t* queue, const xtree_t* options = nullptr) = 0;

		virtual bricks_error_code_e request(const char*, size_t, client_response_cb_t, const xtree_t* options = nullptr) = 0;

	};

	typedef function<void(buffer_t*, xtree_t*)>
		event_cb_t;

	class bidi_plugin_t : public plugin_t
	{
	public:

		virtual bricks_error_code_e init(cb_queue_t *queue, const xtree_t* options = nullptr) = 0;

		virtual bricks_error_code_e register_event_handler(event_cb_t event_cb, const xtree_t* options = nullptr) = 0;

		virtual bricks_error_code_e send_event(const char*, size_t, const xtree_t* options = nullptr) = 0;
	};


}




