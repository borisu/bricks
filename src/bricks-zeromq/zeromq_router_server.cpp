#include "pch.h"
#include "zeromq_router_server.h"


zeromq_router_server_t::zeromq_router_server_t()
{

}

bricks_error_code_e 
zeromq_router_server_t::init(const xtree_t* options)
{
	ZMQ_ASSERT_NOT_INITIATED;

	bricks_error_code_e err = BRICKS_SUCCESS;

	context = zmq_ctx_new();

	router = zmq_socket(context, ZMQ_ROUTER);

	try
	{
		do {

			string url = options->get_property_value_as_string("/configuration/server", "url").value();

			auto rc = zmq_bind(router, url.c_str());
			if (rc != 0)
			{
				err = BRICKS_3RD_PARTY_ERROR;
				break;
			}

			// Create a ZMQ poll item
			items[0] = { router, 0, ZMQ_POLLIN, 0 };

		} while (false);

		initiated = true;

	}
	catch (std::bad_optional_access&)
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
zeromq_router_server_t::register_request_handler(void* opaque, request_cb_t request, const xtree_t* options)
{
	ZMQ_ASSERT_INITIATED;
	ZMQ_ASSERT_NOT_STARTED;

	this->opaque = opaque;
	this->req_cb = request;

	started = true;

	return BRICKS_SUCCESS;

}

zeromq_router_server_t::~zeromq_router_server_t()
{
	destroy();
}

void
zeromq_router_server_t::destroy()
{
	initiated = false;
	started = false;

	if (router)
	{
		zmq_close(router);
		router = nullptr;
	}

	if (context)
	{
		zmq_ctx_destroy(context);
		context = nullptr;
	}
}

bricks_error_code_e 
zeromq_router_server_t::send_response(bricks_handle_t ctx , const char* buf, size_t size, const xtree_t* options)
{

	zmq_msg_send((zmq_msg_t*)ctx, router, ZMQ_SNDMORE);

	zmq_msg_t app_message;
	zmq_msg_init_size(&app_message, size);
	memcpy(zmq_msg_data(&app_message), buf, sizeof(size));
	zmq_msg_send(&app_message, router, 0);

	zmq_msg_close((zmq_msg_t*)ctx);
	zmq_msg_close(&app_message);

	return BRICKS_SUCCESS;

}


bricks_error_code_e
zeromq_router_server_t::poll(size_t timeout)
{
	ZMQ_ASSERT_INITIATED;
	ZMQ_ASSERT_STARTED;

	auto leftover = timeout;

	auto err = BRICKS_TIMEOUT;

	zmq_msg_t *cnx_identity = new zmq_msg_t();
	zmq_msg_init(cnx_identity);

	zmq_msg_t req_data;
	zmq_msg_init(&req_data);

	int more_parts = 0;
	size_t more_parts_size = sizeof(more_parts);

	zmq_poll(items, 1, (long)timeout);

	// If there's data available, receive it
	if (items[0].revents & ZMQ_POLLIN)
	{
		zmq_msg_recv(cnx_identity, router, 0);
		
		zmq_getsockopt(router, ZMQ_RCVMORE, &more_parts, &more_parts_size);

		zmq_msg_recv(&req_data, router, 0);

		auto temp = (const char*)zmq_msg_data(&req_data);

		auto xt = create_xtree();

		req_cb(opaque, (bricks_handle_t)cnx_identity, (const char*)zmq_msg_data(&req_data), zmq_msg_size(&req_data), *xt);

		destroy_xtree(xt);


	}

	zmq_msg_close(&req_data);

	return err;
	
}