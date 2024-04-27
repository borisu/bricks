#pragma once
#include <functional>
#include "common.h"
#include "xtree.h"
#include "queue.h"
#include "buffer.h"

using namespace std;

namespace bricks {

	typedef
		function<void(void*, bricks_error_code_e, xtree_t*)> delivery_cb_t;

	class plugin_t : public brick_t {
	public:

		virtual bricks_error_code_e start() = 0;
	};

	class publisher_plugin_t : public plugin_t
	{
	public:

		virtual bricks_error_code_e init(cb_queue_t *queue, delivery_cb_t msg_cb, const xtree_t* options = nullptr) = 0;

		virtual bricks_error_code_e register_topic(const string& topic, const xtree_t* options = nullptr) = 0;

		virtual bricks_error_code_e publish(const string& topic, const char*, size_t, void* opaque, const xtree_t* options = nullptr) = 0;

	};

	typedef
		function<void(buffer_t*, xtree_t*)> msg_cb_t;

	class subscriber_plugin_t : public plugin_t
	{
	public:

		virtual bricks_error_code_e init(cb_queue_t *queue, msg_cb_t msg_cb, const xtree_t* options = nullptr) = 0;

		virtual bricks_error_code_e register_topic(const string& topic, const xtree_t* options = nullptr) = 0;

	};

	typedef function <void(bricks_error_code_e, const char*, size_t, xtree_t*)>
		response_channel_t;

	typedef function<void(response_channel_t, buffer_t*, xtree_t*)>
		request_cb_t;

	class server_plugin_t : public plugin_t
	{
	public:

		virtual bricks_error_code_e init(cb_queue_t *queue, const xtree_t* options = nullptr) = 0;

		virtual bricks_error_code_e register_request_handler(request_cb_t request, const xtree_t* options = nullptr) = 0;

	};


	typedef function<void(bricks_error_code_e, buffer_t*, xtree_t*)>
		response_cb_t;

	class client_plugin_t : public plugin_t
	{
	public:

		virtual bricks_error_code_e init(cb_queue_t* queue, const xtree_t* options = nullptr) = 0;

		virtual bricks_error_code_e request(const char*, size_t, response_cb_t, const xtree_t* options = nullptr) = 0;

	};

	typedef function<void(void*, const char*, size_t, xtree_t&)> 
		event_cb_t;

	class bidi_plugin_t : public plugin_t
	{
	public:

		virtual bricks_error_code_e init(cb_queue_t *queue, const xtree_t* options = nullptr) = 0;

		virtual bricks_error_code_e register_event_handler(void* opaque, event_cb_t rsp_cb, const xtree_t* options = nullptr) = 0;

		virtual bricks_error_code_e send_event(const char*, size_t, const xtree_t* options = nullptr) = 0;
	};


}





