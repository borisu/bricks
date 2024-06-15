#pragma once
using namespace std;
using namespace bricks;


namespace bricks::plugins {

	class rabbitmq_base_t
	{
	protected:

		virtual bricks_error_code_e handle_connect_options(const string& xtpath, cb_queue_t* queue, const xtree_t* options);

		virtual bricks_error_code_e handle_exchange_declare_options(const string& xtpath, const string& topic, const xtree_t* options);

		virtual bricks_error_code_e handle_queue_declare_options(const string& xtpath, string &queue_name, const xtree_t* options);

		virtual bricks_error_code_e handle_bind_options(const string& xtpath, const string& curr_default_queue_name, const string& topic, const xtree_t* options);

		virtual bricks_error_code_e handle_unbind_options(const string& xtpath, const string& curr_default_queue_name, const string& topic, const xtree_t* options);

		virtual bricks_error_code_e handle_publish_options(const string& xtpath, const string& topic, const char*, size_t, const xtree_t* options);

		void destroy();

		cb_queue_t* queue = nullptr;

		string name;

		string hostname;

		string username;

		string password;

		int port = 5672;

		int channel_max = 0;

		int frame_max = 131072;

		int heartbeat = 0;

		amqp_socket_t* amqp_socket = nullptr;

		amqp_connection_state_t amqp_conn = nullptr;

		int amqp_channel = 1;

	};
}

