#include "pch.h"
#include "zeromq_bidi.h"

using namespace bricks;

using namespace bricks::plugins;

zeromq_bidi_t::zeromq_bidi_t()
{

}

zeromq_bidi_t::~zeromq_bidi_t() 
{
	destroy();
}

bool
zeromq_bidi_t::check_capability(plugin_capabilities_e e)
{
	return false;
}

void
zeromq_bidi_t::set_meta_cb(meta_cb_t meta_cb)
{
	this->meta_cb = meta_cb;
}


void
zeromq_bidi_t::destroy()
{
	destroyed = true;

	stop_zmq_poll_loop();

	if (pair)
	{
		zmq_close(pair);
		pair = nullptr;
	}

	if (context)
	{
		zmq_ctx_destroy(context);
		context = nullptr;
	}
}

bricks_error_code_e
zeromq_bidi_t::init(cb_queue_t* queue, event_cb_t event_cb, const xtree_t* options)
{
	SYNCHRONIZED(mtx);

	ASSERT_PREINIT;

	bricks_error_code_e err = BRICKS_SUCCESS;

	cb_queue = queue;

	context = zmq_ctx_new();

	pair = zmq_socket(context, ZMQ_PAIR);

	try
	{
		name = get<string>(options->get_property_value("/bricks/zmq/bidi", "name").value());

		url = get<string>(options->get_property_value("/bricks/zmq/bidi/methods/init/socket", "url").value());

		is_server = get<bool>(options->get_property_value("/bricks/zmq/bidi/methods/init/socket", "is_server").value());

		int rc = 0;
		if (is_server)
		{
			rc = zmq_bind(pair, url.c_str());
		} 
		else
		{
			rc = zmq_connect(pair, url.c_str());
		}
		
		if (rc != 0) {
			BRICK_LOG_ZMQ_ERROR(zmq_bind);
			throw std::exception();
		}

		if ((err = set_sockopt(options, "/bricks/zmq/bidi/methods/init/socket", pair)) != BRICKS_SUCCESS)
		{
			throw std::exception();
		}

		items[0] = { pair, 0, ZMQ_POLLIN, 0 };

		rc = start_zmq_poll_loop();
		if (rc != BRICKS_SUCCESS)
		{
			throw std::exception();
		}

		this->event_cb = event_cb;

		initiated = true;
	}
	catch (std::bad_optional_access&)
	{
		if (err == BRICKS_SUCCESS)
			err = BRICKS_INVALID_PARAM;
	}
	catch (std::exception&)
	{
		if (err == BRICKS_SUCCESS)
			err = BRICKS_3RD_PARTY_ERROR;
	}

	if (err != BRICKS_SUCCESS)
	{
		destroy();
	}

	return err;


}

bricks_error_code_e
zeromq_bidi_t::send_event(const char* buf, size_t size, const xtree_t* options)
{
	SYNCHRONIZED(mtx);

	ASSERT_READY;

	auto rc = zmq_send(pair, buf, size, 0);
	if (rc == -1)
		BRICK_LOG_ZMQ_ERROR(zmq_send);

	return rc == -1 ? BRICKS_3RD_PARTY_ERROR : BRICKS_SUCCESS ;

}


bricks_error_code_e
zeromq_bidi_t::do_zmq_poll(int milliseconds, bool last_call)
{

	if (last_call)
		return BRICKS_SUCCESS;

	// Poll for incoming data
	int rc = zmq_poll(items, 1, (long)milliseconds);
	if (rc == -1)
	{
		BRICK_LOG_ZMQ_ERROR(zmq_poll);
		return BRICKS_3RD_PARTY_ERROR;
	}

	// If there's data available, receive it
	if (items[0].revents & ZMQ_POLLIN) {

		int more = 0;
		size_t more_size = sizeof(more);

		do {

			zmq_msg_t msg;
			zmq_msg_init(&msg);
			rc = zmq_msg_recv(&msg, pair, 0);
			if (rc == -1)
			{
				BRICK_LOG_ZMQ_ERROR(zmq_msg_recv);
				return BRICKS_3RD_PARTY_ERROR;
			}

			if (zmq_getsockopt(pair, ZMQ_RCVMORE, &more, &more_size) != 0)
			{
				BRICK_LOG_ZMQ_ERROR(zmq_msg_recv);
				return BRICKS_3RD_PARTY_ERROR;
			}

			string opt = "";

			auto xt = create_xtree_from_xml((opt +
				"<bricks>" +
				"  <event last=\"" + (more ? "true" : "false") + "\"/>" +
				"</bricks>").c_str()
			);

			
			cb_queue->enqueue(
				std::bind(event_cb, create_buffer((const char*)zmq_msg_data(&msg), (int)zmq_msg_size(&msg)), xt)
			);

			

		} while (more);


		return BRICKS_SUCCESS;

	}

	return BRICKS_TIMEOUT;
	
}

void
zeromq_bidi_t::do_destroy()
{
	SYNCHRONIZED(mtx);

	destroy();

	if (meta_cb)
		this->cb_queue->enqueue(std::bind(meta_cb, OBJECT_DESTROYED, nullptr));
}