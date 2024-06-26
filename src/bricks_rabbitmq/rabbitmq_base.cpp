#include "pch.h"
#include "rabbitmq_base.h"
#include "rabbitmq_utils.h"

using namespace std;
using namespace bricks;
using namespace bricks::plugins;

void
rabbitmq_base_t::destroy()
{
	if (amqp_conn)
	{
		amqp_channel_close(amqp_conn, amqp_channel, AMQP_REPLY_SUCCESS);
		amqp_connection_close(amqp_conn, AMQP_REPLY_SUCCESS);
		amqp_destroy_connection(amqp_conn);

		amqp_conn = nullptr;
	}
}

bricks_error_code_e
rabbitmq_base_t::handle_connect_options(const string& prefix, cb_queue_t* queue, const xtree_t* options)
{

	this->queue = queue;

	bricks_error_code_e err = BRICKS_SUCCESS;

	try
	{
		do {
			
			/* mandatory */
			hostname = get<string>(options->get_property_value(prefix.c_str(), "hostname").value());
			port = get<int>(options->get_property_value(prefix.c_str(), "port").value());
			string method = get<string>(options->get_property_value((prefix + "/login").c_str(), "method").value());
			if (method != "plain")
			{
				err = BRICKS_INVALID_PARAM;
				break;
			}
			username = get<string>(options->get_property_value((prefix + "/login").c_str(), "username").value());
			password = get<string>(options->get_property_value((prefix + "/login").c_str(), "password").value());


			/* optional */
			channel_max = get_opt<int>(options->get_property_value((prefix + "/login").c_str(), "channel_max"), 0);
			frame_max   = get_opt<int>(options->get_property_value((prefix + "/login").c_str(), "frame_max"), 131072);
			heartbeat   = get_opt<int>(options->get_property_value((prefix + "/login").c_str(), "heartbeat"), 0);

			bricks_error_code_e err = BRICKS_SUCCESS;

			amqp_conn = amqp_new_connection();

			amqp_socket = amqp_tcp_socket_new(amqp_conn);
			if (!amqp_socket) {
				log1(BRICKS_ALARM, "%s %%%%%% Error creating TCP socket", name.c_str());
			}

			int status = amqp_socket_open(amqp_socket, hostname.c_str(), port);
			if (status) {
				log1(BRICKS_ALARM, "%s %%%%%% Error opening TCP socket", name.c_str());
			}

			if (BRICKS_SUCCESS != (err = check_amqp_error(amqp_login(amqp_conn, "/", channel_max, frame_max, heartbeat, AMQP_SASL_METHOD_PLAIN,
				username.c_str(), password.c_str()),
				name.c_str(), "amqp_login"))){
				err = BRICKS_3RD_PARTY_ERROR;
				break;
			}

			amqp_channel_open(amqp_conn, 1);

			if (BRICKS_SUCCESS != (err = check_amqp_error(amqp_get_rpc_reply(amqp_conn), name.c_str(), "amqp_get_rpc_reply")))
			{
				err = BRICKS_3RD_PARTY_ERROR;
				break;
			}

		} while (false);
	}
	catch (std::bad_optional_access&)
	{
		err = BRICKS_INVALID_PARAM;
	}
	catch (std::bad_variant_access&)
	{
		err = BRICKS_INVALID_PARAM;
	}

	if (err != BRICKS_SUCCESS)
	{
		destroy();
	}

	return err;
}



bricks_error_code_e 
rabbitmq_base_t::handle_queue_declare_options(const string& xtpath, string& out_queue_name, const xtree_t* options)
{
	bricks_error_code_e err = BRICKS_SUCCESS;

	string queue_name  = "";
	bool passive = false;
	bool durable = false;
	bool auto_delete = false;
	bool exclusive = false;
	bool declare = true;

	if (options)
	{
		queue_name  = get_opt<string>(options->get_property_value(xtpath.c_str(), "name"), "");
		declare     = get_opt<bool>(options->get_property_value(xtpath.c_str(), "declare"), false);
		passive     = get_opt<bool>(options->get_property_value(xtpath.c_str(), "passive"), false);
		durable     = get_opt<bool>(options->get_property_value(xtpath.c_str(), "durable"), false);
		auto_delete = get_opt<bool>(options->get_property_value(xtpath.c_str(), "auto_delete"), true);
		exclusive   = get_opt<bool>(options->get_property_value(xtpath.c_str(), "exclusive"), false);
	}

	if (!declare)
		return err;
		
	auto declare_ok = amqp_queue_declare(amqp_conn, amqp_channel, amqp_cstring_bytes(queue_name.c_str()), passive, durable, exclusive, auto_delete, amqp_empty_table);

	if (BRICKS_SUCCESS != (err = check_amqp_error(amqp_get_rpc_reply(amqp_conn), name.c_str(), "amqp_queue_declare/amqp_get_rpc_reply")))
	{
		return err;
	}
	out_queue_name = string((char*)declare_ok->queue.bytes, declare_ok->queue.len);

	auto consume_ok = amqp_basic_consume(amqp_conn, 1, amqp_cstring_bytes(queue_name.c_str()), amqp_empty_bytes, 0, 1, 0,amqp_empty_table);
	if (BRICKS_SUCCESS != (err = check_amqp_error(amqp_get_rpc_reply(amqp_conn), name.c_str(), "amqp_basic_consume/amqp_get_rpc_reply")))
	{
		return err;
	}

	

	return err;
}

bricks_error_code_e 
rabbitmq_base_t::handle_bind_options(const string& xtpath, const string& default_queue_name, const string& topic, const xtree_t* options)
{
	bricks_error_code_e err = BRICKS_SUCCESS;

	string binding_key;
	string queue_name = default_queue_name;

	if (options)
	{
		binding_key = get_opt<string>(options->get_property_value(xtpath.c_str(), "binding_key"), "");
		queue_name        = get_opt<string>(options->get_property_value(xtpath.c_str(), "name"), "");
		if (queue_name == "")
			queue_name = default_queue_name;

	}

	auto ok = amqp_queue_bind(amqp_conn, amqp_channel, amqp_cstring_bytes(queue_name.c_str()), amqp_cstring_bytes(topic.c_str()),
		amqp_cstring_bytes(binding_key.c_str()), amqp_empty_table);

	if (BRICKS_SUCCESS != (err = check_amqp_error(amqp_get_rpc_reply(amqp_conn), name.c_str(), "amqp_queue_bind/amqp_get_rpc_reply")))
	{
		return err;
	}

	return err;
}



bricks_error_code_e
rabbitmq_base_t::handle_unbind_options(const string& xtpath, const string& default_queue_name, const string& topic, const xtree_t* options)
{
	bricks_error_code_e err = BRICKS_SUCCESS;

	string binding_key;
	string queue_name = default_queue_name;
	
	if (options)
	{
			binding_key = get_opt<string>(options->get_property_value(xtpath.c_str(), "binding_key"), "");
			queue_name = get_opt<string>(options->get_property_value(xtpath.c_str(), "name"), "");
			if (queue_name == "")
				queue_name = default_queue_name;
	}
	

	amqp_queue_unbind(amqp_conn, amqp_channel, amqp_cstring_bytes(queue_name.c_str()), amqp_cstring_bytes(topic.c_str()),
		amqp_cstring_bytes(binding_key.c_str()), amqp_empty_table);

	if (BRICKS_SUCCESS != (err = check_amqp_error(amqp_get_rpc_reply(amqp_conn), name.c_str(), "amqp_queue_unbind/amqp_get_rpc_reply")))
	{
		err = BRICKS_3RD_PARTY_ERROR;
	}

	return err;
}


bricks_error_code_e 
rabbitmq_base_t::handle_exchange_declare_options(const string& xt_path, const string& topic, const xtree_t* options)
{
	bricks_error_code_e err = BRICKS_SUCCESS;

	string type = "topic";
	bool passive = false;
	bool durable = false;
	bool auto_delete = false;
	bool internal = false;
	bool declare = false;

	if (options)
	{
		declare = get_opt<bool>(options->get_property_value(xt_path.c_str(), "declare"), false);
		if (!declare)
			return BRICKS_SUCCESS;

		type = get_opt<string>(options->get_property_value(xt_path.c_str(), "type"), "topic");
		passive = get_opt<bool>(options->get_property_value(xt_path.c_str(), "passive"), false);
		durable = get_opt<bool>(options->get_property_value(xt_path.c_str(), "durable"), false);
		auto_delete = get_opt<bool>(options->get_property_value(xt_path.c_str(), "auto_delete"), false);
		internal = get_opt<bool>(options->get_property_value(xt_path.c_str(), "internal"), false);

	}
	
	amqp_exchange_declare(amqp_conn, 1, amqp_cstring_bytes(topic.c_str()),
		amqp_cstring_bytes(type.c_str()), passive, durable, auto_delete, internal,
		amqp_empty_table);

	if (BRICKS_SUCCESS != (err = check_amqp_error(amqp_get_rpc_reply(amqp_conn), name.c_str(), "amqp_exchange_declare/amqp_get_rpc_reply")))
	{
		return err;
	}
	
	return err;

}


bricks_error_code_e 
rabbitmq_base_t::handle_publish_options(const string& publish_xtpath, const string& topic, const char* data, size_t size, const xtree_t* options)
{
	bricks_error_code_e err = BRICKS_SUCCESS;

	string queue;
	string routing_key;
	bool mandatory = false;
	bool immediate = false;

	if (options)
	{
		queue = get_opt<string>(options->get_property_value(publish_xtpath.c_str(), "queue"));
		routing_key = get_opt<string>(options->get_property_value(publish_xtpath.c_str(), "routing_key"));
		mandatory = get_opt<bool>(options->get_property_value(publish_xtpath.c_str(), "mandatory"));
		immediate = get_opt<bool>(options->get_property_value(publish_xtpath.c_str(), "immediate"));
	}

	err = check_error(amqp_basic_publish(amqp_conn, 1, amqp_cstring_bytes(topic.c_str()),
		amqp_cstring_bytes(queue.c_str()), mandatory, immediate, NULL,
		amqp_bytes_t{ size, (void*)data }), name.c_str(), "amqp_basic_publish");

	return err;
}