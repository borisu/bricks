#include "pch.h"
#include "rabbitmq_subscriber.h"
#include "rabbitmq_plugin.h"
#include "rabbitmq_utils.h"
#include <time.h>

using namespace std;
using namespace bricks;
using namespace bricks::plugins;
using namespace std::placeholders;



subscriber_plugin_t*
bricks::plugins::create_rabbitmq_subscriber()
{
	return new rabbitmq_subscriber_t();
}

rabbitmq_subscriber_t::rabbitmq_subscriber_t()
{

}

rabbitmq_subscriber_t::~rabbitmq_subscriber_t()
{
	SYNCHRONIZED(mtx);

	destroy();
}

void
rabbitmq_subscriber_t::destroy()
{
	SYNCHRONIZED(mtx);

	rabbitmq_base_t::destroy();
}

bricks_error_code_e
rabbitmq_subscriber_t::init(cb_queue_t* queue, topic_cb_t msg_cb, const xtree_t* options)
{

	SYNCHRONIZED(mtx);

	ASSERT_NOT_INITIATED;

	this->msg_cb = msg_cb;

	bool declare = true;
	bricks_error_code_e err = BRICKS_SUCCESS;

	do {

		if (BRICKS_SUCCESS != (err = rabbitmq_base_t::handle_connect_options("/bricks/rabbitmq/subscriber", queue, options)))
		{
			break;
		}

		if (BRICKS_SUCCESS != (err = rabbitmq_base_t::handle_queue_declare_options("/bricks/rabbitmq/subscriber/init/queue", this->default_queue_name, options)))
		{
			break;
		}

		initiated = true;

	} while (false);

	if (err != BRICKS_SUCCESS)
	{
		destroy();
	}

	return err;
}

bool 
rabbitmq_subscriber_t::check_capability(plugin_capabilities_e e)
{
	SYNCHRONIZED(mtx);

	return false;
}

bricks_error_code_e 
rabbitmq_subscriber_t::subscribe(const string& topic, const xtree_t* options)
{
	SYNCHRONIZED(mtx);

	ASSERT_INITIATED;
	ASSERT_NOT_STARTED;

	bricks_error_code_e err = BRICKS_SUCCESS;

	err = handle_exchange_declare_options("/bricks/rabbitmq/subscriber/exchange", topic, options);
	if (err != BRICKS_SUCCESS)
		return err;

	err = rabbitmq_base_t::handle_queue_declare_options("/bricks/rabbitmq/subscriber/queue", this->default_queue_name, options);
	if (err != BRICKS_SUCCESS)
		return err;

	err = rabbitmq_base_t::handle_bind_options("/bricks/rabbitmq/subscriber/bind", this->default_queue_name, topic, options);
	return err;

}

bricks_error_code_e 
rabbitmq_subscriber_t::unsubscribe(const string& topic, const xtree_t* options)
{
	SYNCHRONIZED(mtx);

	ASSERT_INITIATED;
	ASSERT_NOT_STARTED;

	bricks_error_code_e err = BRICKS_SUCCESS;

	err = rabbitmq_base_t::handle_unbind_options("/bricks/rabbitmq/subscriber/queue", this->default_queue_name, topic, options);
	return err;

	return err;
}

bricks_error_code_e  
rabbitmq_subscriber_t::unsubscribe(const xtree_t* options)
{
	SYNCHRONIZED(mtx);

	ASSERT_INITIATED;

	bricks_error_code_e err = BRICKS_SUCCESS;
	
	err = rabbitmq_base_t::handle_unbind_options("/bricks/rabbitmq/subscriber/queue", this->default_queue_name, "*", options);

	return err;
}


bricks_error_code_e 
rabbitmq_subscriber_t::start()
{
	SYNCHRONIZED(mtx);

	ASSERT_INITIATED;
	ASSERT_NOT_STARTED;

	rmq_poll_thread = new thread(&rabbitmq_subscriber_t::rmq_poll_loop, this);

	started = true;

	return BRICKS_SUCCESS;

}

void
rabbitmq_subscriber_t::on_topic(std::string pattern, std::string channel, std::string msg)
{
	SYNCHRONIZED(mtx);

	string opt = "";

	auto xt = create_xtree_from_xml((opt +
		"<bricks>" +
		"  <event last=\"true\"/>" +
		"</bricks>").c_str()
	);


	this->queue->enqueue(
		std::bind(
			msg_cb,
			channel,
			create_buffer((const char*)msg.data(), (int)msg.size()),
			xt)
	);
}


void
rabbitmq_subscriber_t::rmq_poll_loop()
{
	amqp_frame_t frame;

	timeval t;
	t.tv_sec = 0;
	t.tv_usec = 500000;


	for (;;) {
		SYNCHRONIZED(mtx);
		{
			mtx.unlock();
			std::this_thread::yield(); // give chance for subscribe unsubscribe
			mtx.lock();
		}

		amqp_rpc_reply_t ret;
		amqp_envelope_t envelope;

		amqp_maybe_release_buffers(amqp_conn);
		ret = amqp_consume_message(amqp_conn, &envelope, &t, 0);
		

		if (AMQP_RESPONSE_NORMAL != ret.reply_type) {
			if (AMQP_RESPONSE_LIBRARY_EXCEPTION == ret.reply_type &&
				AMQP_STATUS_UNEXPECTED_STATE == ret.library_error) {
				if (AMQP_STATUS_OK != amqp_simple_wait_frame(amqp_conn, &frame)) {
					return;
				}

				if (AMQP_FRAME_METHOD == frame.frame_type) {
					switch (frame.payload.method.id) {
					case AMQP_BASIC_ACK_METHOD:
						/* if we've turned publisher confirms on, and we've published a
						 * message here is a message being confirmed.
						 */
						break;
					case AMQP_BASIC_RETURN_METHOD:
						/* if a published message couldn't be routed and the mandatory
						 * flag was set this is what would be returned. The message then
						 * needs to be read.
						 */
					{
						amqp_message_t message;
						ret = amqp_read_message(amqp_conn, frame.channel, &message, 0);
						if (AMQP_RESPONSE_NORMAL != ret.reply_type) {
							return;
						}

						amqp_destroy_message(&message);
					}

					break;

					case AMQP_CHANNEL_CLOSE_METHOD:
						/* a channel.close method happens when a channel exception occurs,
						 * this can happen by publishing to an exchange that doesn't exist
						 * for example.
						 *
						 * In this case you would need to open another channel redeclare
						 * any queues that were declared auto-delete, and restart any
						 * consumers that were attached to the previous channel.
						 */
						return;

					case AMQP_CONNECTION_CLOSE_METHOD:
						/* a connection.close method happens when a connection exception
						 * occurs, this can happen by trying to use a channel that isn't
						 * open for example.
						 *
						 * In this case the whole connection must be restarted.
						 */
						return;

					default:
						fprintf(stderr, "An unexpected method was received %u\n",
							frame.payload.method.id);
						return;
					}
				}
			}

		}
		else {
			amqp_destroy_envelope(&envelope);
		}

	}
	return ;

}
