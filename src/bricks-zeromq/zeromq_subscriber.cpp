#include "pch.h"
#include "zeromq_subscriber.h"

using namespace bricks;
using namespace bricks::plugins;


zeromq_subscriber_t::zeromq_subscriber_t()
{

};

void
zeromq_subscriber_t::destroy()
{
	initiated = false;
	started = false;

	stop_zmq_poll_loop();

	if (subscriber)
	{
		zmq_close(subscriber);
		subscriber = nullptr;
	}

	if (context)
	{
		zmq_ctx_destroy(context);
		context = nullptr;
	}
}

bricks_error_code_e
zeromq_subscriber_t::init(cb_queue_t* queue, msg_cb_t msg_cb, const xtree_t* options)
{
	ASSERT_NOT_INITIATED;
	ASSERT_NOT_STARTED;

	bricks_error_code_e err = BRICKS_SUCCESS;

	this->cb_queue = queue;

	context    = zmq_ctx_new();

	subscriber = zmq_socket(context, ZMQ_SUB);

	try
	{
		name = get<string>(options->get_property_value("/bricks/zmq/subscriber", "name").value());

		url = get<string>(options->get_property_value("/bricks/zmq/subscriber", "url").value());

		// Connect to the ZeroMQ endpoint
		auto rc = zmq_connect(subscriber, url.c_str());
		if (rc != 0) {
			throw std::exception();
		}

		this->msg_cb = msg_cb;

		// Create a ZMQ poll item
		items[0] = { subscriber, 0, ZMQ_POLLIN, 0 };

		set_sockopt(options, "/bricks/zmq/subscriber", subscriber);

		initiated = true;
		
	}
	catch (std::bad_optional_access&)
	{
		err = BRICKS_INVALID_PARAM;
	}
	catch (std::exception&)
	{
		err = BRICKS_FAILURE_GENERIC;
	}

	if (err != BRICKS_SUCCESS)
	{
		destroy();
	}

	return err;

}

zeromq_subscriber_t::~zeromq_subscriber_t()
{
	destroy();
}

bricks_error_code_e
zeromq_subscriber_t::register_topic(const string& topic, const xtree_t* options)
{
	ASSERT_INITIATED;
	ASSERT_NOT_STARTED;

	auto rc = zmq_setsockopt(subscriber, ZMQ_SUBSCRIBE, topic.c_str(), strlen(topic.c_str()));
	if (rc != 0)
	{
		BRICK_LOG_ZMQ_ERROR(zmq_setsockopt);
		return  BRICKS_3RD_PARTY_ERROR;
	}
	
	return BRICKS_SUCCESS;

}

bricks_error_code_e 
zeromq_subscriber_t::start()
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
zeromq_subscriber_t::do_zmq_poll(int milliseconds, bool last_call)
{

	ASSERT_INITIATED;
	ASSERT_STARTED;

	if (last_call)
		return BRICKS_SUCCESS;

	// Poll for incoming data
	int rc = zmq_poll(items, 1, (long)milliseconds);
	if (rc == -1)
	{
		BRICK_LOG_ZMQ_ERROR(zmq_poll);
		started = false;
		return BRICKS_3RD_PARTY_ERROR;
	}

	// If there's data available, receive it
	if (items[0].revents & ZMQ_POLLIN) {

		int count = 0;
		string topic = "";
		int more = 0;
		size_t more_size = sizeof(more);

		do {

			zmq_msg_t msg;
			zmq_msg_init(&msg);
			rc = zmq_msg_recv(&msg, subscriber, 0);
			if (rc == -1)
			{
				BRICK_LOG_ZMQ_ERROR(zmq_msg_recv);
				return BRICKS_3RD_PARTY_ERROR;
			}

			if (count == 0)
			{
				topic.assign((const char*)zmq_msg_data(&msg), (int)zmq_msg_size(&msg));
				count++;
				if (zmq_getsockopt(subscriber, ZMQ_RCVMORE, &more, &more_size) != 0)
				{
					BRICK_LOG_ZMQ_ERROR(zmq_msg_recv);
					return BRICKS_3RD_PARTY_ERROR;
				}
				continue;
			}

			if (zmq_getsockopt(subscriber, ZMQ_RCVMORE, &more, &more_size) != 0)
			{
				BRICK_LOG_ZMQ_ERROR(zmq_msg_recv);
				return BRICKS_3RD_PARTY_ERROR;
			}

			count++;

			string opt = "";

			auto xt = create_xtree_from_xml((opt +
				"<bricks>" + 
				"  <event last=\"" +  (more ? "true" : "false")  +  "\"/>" +
				"</bricks>").c_str()
			);

			if (cb_queue)
			{
				cb_queue->enqueue(
					std::bind(
						msg_cb,
						topic,
						create_buffer((const char*)zmq_msg_data(&msg), (int)zmq_msg_size(&msg)),
						xt)
				);
			} else {
				try { 
					msg_cb(
						topic, 
						create_buffer((const char*)zmq_msg_data(&msg), (int)zmq_msg_size(&msg)), 
						xt); 
				}
				catch (std::exception&) {};
			}
		} while (more);
			

		return BRICKS_SUCCESS;

	}

	return BRICKS_TIMEOUT;
}

