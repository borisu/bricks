#pragma once
#include <functional>
#include "common.h"
#include "xtree.h"

using namespace std;

namespace bricks {

	typedef
		function<void(void*, bricks_error_code_e, xtree_t*)> delivery_cb_t;

	class plugin_t
	{
	public:
		virtual bricks_error_code_e poll(int milliseconds) = 0;
	};

	class publisher_plugin_t : public plugin_t
	{
	public:

		virtual bricks_error_code_e init(delivery_cb_t msg_cb, const xtree_t* options = nullptr) = 0;

		virtual bricks_error_code_e register_topic(const string& topic, const xtree_t* options = nullptr) = 0;

		virtual bricks_error_code_e publish(const string& topic, const char*, size_t, void* opaque, const xtree_t* options = nullptr) = 0;

	};

	typedef
		function<void(void*, const char*, size_t, xtree_t *)> msg_cb_t;

	class subscriber_plugin_t : public plugin_t
	{
	public:

		virtual bricks_error_code_e init(msg_cb_t msg_cb, const xtree_t* options = nullptr) = 0;

		virtual bricks_error_code_e register_topic(const string& topic, const xtree_t* options = nullptr) = 0;

		virtual bricks_error_code_e subscribe(void* opaque, const xtree_t* options = nullptr) = 0;

	};

	typedef
		function<void(void*, bricks_handle_t, const char*, size_t, const xtree_t&)> request_cb_t;

	class server_plugin_t : public plugin_t
	{
	public:

		virtual bricks_error_code_e init(const xtree_t* options = nullptr) = 0;

		virtual bricks_error_code_e register_request_handler(void* opaque, request_cb_t request, const xtree_t* options = nullptr) = 0;

		virtual bricks_error_code_e send_response(bricks_handle_t ctx, const char*, size_t, const xtree_t* options = nullptr) = 0;

	};

	typedef
		function<void(void*, const char*, size_t, xtree_t&)> event_cb_t;

	class bidi_plugin_t : public plugin_t
	{
	public:

		virtual bricks_error_code_e init(const xtree_t* options = nullptr) = 0;

		virtual bricks_error_code_e register_event_handler(void* opaque, event_cb_t rsp_cb, const xtree_t* options = nullptr) = 0;

		virtual bricks_error_code_e send_event(const char*, size_t, const xtree_t* options = nullptr) = 0;
	};


	class selector_t
	{
		virtual bricks_error_code_e init(std::vector<plugin_t*> &plugins) = 0;

		virtual bricks_error_code_e poll(int milliseconds) = 0;

	};
	

}





