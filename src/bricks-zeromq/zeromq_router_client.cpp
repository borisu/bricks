#include "pch.h"
#include "zeromq_router_client.h"

zeromq_router_client_t::zeromq_router_client_t()
{

}

bricks_error_code_e
zeromq_router_client_t::init(const xtree_t* options)
{
	ZMQ_ASSERT_NOT_INITIATED;

	bricks_error_code_e err = BRICKS_SUCCESS;

	context = zmq_ctx_new();

	router = zmq_socket(context, ZMQ_ROUTER);

	try
	{
		do {

			string url = options->get_property_value_as_string("/configuration/client", "url").value();

			auto rc = zmq_connect(router, url.c_str());

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
zeromq_router_client_t::issue_request(const string& request_id, void* opaque, const char*, size_t, const xtree_t* options)
{
	return BRICKS_SUCCESS;

	/*zmq_msg_t response_msg;
	zmq_msg_init_size(&response_msg, size);
	memcpy(zmq_msg_data(&response_msg), response, size);

	zmq_msg_send((zmq_msg_t*)identity, router, ZMQ_SNDMORE);
	zmq_msg_send(&response_msg, router, 0);

	zmq_msg_close((zmq_msg_t*)identity);
	zmq_msg_close(&response_msg);*/

	return BRICKS_SUCCESS;
}

bricks_error_code_e
zeromq_router_client_t::register_client(response_cb_t request, const xtree_t* options)
{
	ZMQ_ASSERT_INITIATED;
	ZMQ_ASSERT_NOT_STARTED;

	this->opaque = opaque;
	this->req_cb = request;

	return BRICKS_SUCCESS;

}

zeromq_router_client_t::~zeromq_router_client_t()
{
	destroy();
}

void
zeromq_router_client_t::destroy()
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
zeromq_router_client_t::poll(size_t timeout)
{
	return BRICKS_SUCCESS;

	/*ZMQ_ASSERT_INITIATED;
	ZMQ_ASSERT_STARTED;

	auto leftover = timeout;

	auto err = BRICKS_TIMEOUT;

	while (leftover > 0)
	{
		// Poll for incoming data
		auto start = std::chrono::steady_clock::now();
		zmq_poll(items, 1, (long)timeout);
		auto end = std::chrono::steady_clock::now();

		leftover = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

		// If there's data available, receive it
		if (items[0].revents & ZMQ_POLLIN)
		{
			if (rcv_state == NEXT_IS_IDENTITY)
			{
				identity = new zmq_msg_t();
				zmq_msg_init(identity);
				zmq_msg_recv(identity, router, 0);
				rcv_state = NEXT_IS_MESSAGE;
			}
			else
			{
				request = new zmq_msg_t();
				zmq_msg_init(request);
				zmq_msg_recv(request, router, 0);

				auto xt = create_xtree();

				req_cb(opaque, BRICKS_SUCCESS, (bricks_handle_t)identity, (const char*)zmq_msg_data(request), zmq_msg_size(request), *xt);

				destroy_xtree(xt);

				zmq_msg_close(request);

				delete request;

				rcv_state = NEXT_IS_IDENTITY;
				identity = nullptr;
				request = nullptr;

				err = BRICKS_SUCCESS;
				break;

			}

		}
		else
		{
			err = BRICKS_TIMEOUT;
			break;
		}
	}

	return err;*/

}