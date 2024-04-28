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

void
zeromq_bidi_t::destroy()
{
	initiated = false;
	started = false;

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
zeromq_bidi_t::init(cb_queue_t* queue, const xtree_t* options)
{
	ASSERT_NOT_INITIATED;

	bricks_error_code_e err = BRICKS_SUCCESS;

	cb_queue = queue;

	context = zmq_ctx_new();

	pair = zmq_socket(context, ZMQ_PAIR);

	try
	{
		url = get<string>(options->get_property_value("/bricks/zmq/bidi", "url").value());

		is_server = get<bool>(options->get_property_value("/bricks/zmq/bidi", "is_server").value());
		
		int rc = is_server ? zmq_bind(pair, url.c_str()) : zmq_connect(pair, url.c_str());

		if (rc != 0) {
			throw std::exception();
		}

		items[0] = { pair, 0, ZMQ_POLLIN, 0 };

		initiated = true;
	}
	catch (std::bad_optional_access&)
	{
		err = BRICKS_INVALID_PARAM;
	}
	catch (std::exception&)
	{
		err = BRICKS_3RD_PARTY_ERROR;
	}


	if (err != BRICKS_SUCCESS)
	{
		destroy();
	}

	return err;


}

bricks_error_code_e 
zeromq_bidi_t::register_event_handler( event_cb_t event_cb, const xtree_t* options)
{
	ASSERT_INITIATED;
	ASSERT_NOT_STARTED;

	this->event_cb = event_cb;

	return BRICKS_SUCCESS;

}

bricks_error_code_e
zeromq_bidi_t::send_event(const char* buf, size_t size, const xtree_t* options)
{
	ASSERT_INITIATED;
	ASSERT_STARTED;

	auto rc = zmq_send(pair, buf, size, 0);

	return rc == -1 ? BRICKS_3RD_PARTY_ERROR : BRICKS_SUCCESS ;

}

bricks_error_code_e
zeromq_bidi_t::start()
{
	ASSERT_INITIATED;
	ASSERT_NOT_STARTED;

	auto rc = start_zmq_poll_loop();
	if (rc == BRICKS_SUCCESS)
	{
		started = true;
	}

	return rc;

}

bricks_error_code_e
zeromq_bidi_t::do_zmq_poll(int milliseconds, bool last_call)
{

	ASSERT_INITIATED;
	ASSERT_STARTED;

	// Poll for incoming data
	zmq_poll(items, 1, (long)milliseconds);

	// If there's data available, receive it
	if (items[0].revents & ZMQ_POLLIN) {

		zmq_msg_t msg;
		zmq_msg_init(&msg);
		zmq_msg_recv(&msg, pair, 0);

		auto xt = create_xtree();

		auto buf = create_buffer((const char*)zmq_msg_data(&msg), (int)zmq_msg_size(&msg));

		if (cb_queue) 
		{ 
			cb_queue->enqueue(
				std::bind(event_cb, buf, xt)
			);
		}
		else
		{
			try { event_cb(buf, xt); }
			catch (std::exception&) {};
		}
		
		return BRICKS_SUCCESS;
	}

	return BRICKS_TIMEOUT;
	
}