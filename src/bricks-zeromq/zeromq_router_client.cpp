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

	int rc = 0;

	try
	{
		do {

			string url = options->get_property_value_as_string("/configuration/client", "url").value();

			rc = zmq_connect(router, url.c_str());

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
		if (err != 0)
			log1(BRICKS_DEBUG, "zmq error(%d): %s", rc, zmq_strerror(rc));

		destroy();
	}

	return err;
}

bricks_error_code_e 
zeromq_router_client_t::issue_request(guid_t guid, const char*, size_t size, const xtree_t* options)
{
	bricks_error_code_e err = BRICKS_SUCCESS;

	zmq_msg_t identity;
	zmq_msg_t app_message;
	zmq_msg_init(&identity);
	zmq_msg_init(&app_message);

	int rc = 0;

	do {

		zmq_msg_close(&identity);
		rc = zmq_msg_init_size(&identity, sizeof(guid));
		if (rc != 0) {
			err = BRICKS_3RD_PARTY_ERROR; break;
		};

		memcpy(zmq_msg_data(&identity), guid, sizeof(guid));

		rc = zmq_msg_send(&identity, router, ZMQ_SNDMORE);
		if (rc  <=0 ) {
			err = BRICKS_3RD_PARTY_ERROR; 
			break;
		};

		zmq_msg_close(&app_message);
		rc = zmq_msg_init_size(&app_message, size);
		if (rc != 0) {
			err = BRICKS_3RD_PARTY_ERROR; break;
		};

		rc = zmq_msg_send(&app_message, router, 0);
		if (rc <= 0) {
			err = BRICKS_3RD_PARTY_ERROR; break;
		};

	} while (false);

	if (err != 0)
		log1(BRICKS_DEBUG, "zmq error(%d): %s", zmq_errno(), zmq_strerror(rc));

	zmq_msg_close(&identity);
	zmq_msg_close(&app_message);

	return err;
}

bricks_error_code_e
zeromq_router_client_t::register_client(void* opaque, response_cb_t request, const xtree_t* options)
{
	ZMQ_ASSERT_INITIATED;
	ZMQ_ASSERT_NOT_STARTED;

	this->opaque = opaque;
	this->rsp_cb = request;

	started = true;

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
	ZMQ_ASSERT_INITIATED;
	ZMQ_ASSERT_STARTED;

	auto leftover = timeout;

	auto err = BRICKS_TIMEOUT;

	int rc = 0;


	do {

		zmq_poll(items, 1, (long)timeout);

		// If there's data available, receive it
		if (items[0].revents & ZMQ_POLLIN)
		{
			auto identity = new zmq_msg_t();
			zmq_msg_init(identity);
			zmq_msg_recv(identity, router, 0);

			int more_parts;
			size_t more_parts_size = sizeof(more_parts);
			zmq_getsockopt(router, ZMQ_RCVMORE, &more_parts, &more_parts_size);

			guid_t guid;
			memcpy(&guid, zmq_msg_data(identity), sizeof(guid_t));


			auto app_message = new zmq_msg_t();
			zmq_msg_init(app_message);
			zmq_msg_recv(app_message, router, 0);

			auto xt = create_xtree();

			rsp_cb(opaque, guid, (const char*)zmq_msg_data(app_message), zmq_msg_size(app_message), *xt);

			destroy_xtree(xt);

			zmq_msg_close(identity);
			zmq_msg_close(app_message);

			err = BRICKS_SUCCESS;

		}
	} while (false);


	if (err != 0)
		log1(BRICKS_DEBUG, "zmq error(%d): %s", zmq_errno(), zmq_strerror(rc));

	return err;

}