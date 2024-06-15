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

	shutdown = true;

	if (rmq_poll_thread)
		rmq_poll_thread->join();

	delete rmq_poll_thread;

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

		name = get_opt<string>(options->get_property_value("/bricks/rabbitmq/subscriber", "name"));

		if (BRICKS_SUCCESS != (err = rabbitmq_base_t::handle_connect_options("/bricks/rabbitmq/subscriber/init/connection", queue, options)))
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
rabbitmq_subscriber_t::rmq_poll_loop()
{
	amqp_frame_t frame;

	timeval t;
	t.tv_sec = 0;
	t.tv_usec = 500000;


	for (;!shutdown;) {
		SYNCHRONIZED(mtx);

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
						log1(BRICKS_TRACE, "An unexpected method was received %u\n",
							frame.payload.method.id);
						return;
					}
				}
			}

		}
		else {
			/*string opt = "";

			auto xt = create_xtree_from_xml((opt +
				"<bricks>" +
				"  <event last=\"true\"/>" +
				"</bricks>").c_str()
			);


			this->queue->enqueue(
				std::bind(
					msg_cb,
					"",
					create_buffer((const char*)message.body.bytes, (int)message.body.len),
					xt));

			amqp_destroy_message(&message);*/

			amqp_destroy_envelope(&envelope);
		}

	}
	return ;

}
