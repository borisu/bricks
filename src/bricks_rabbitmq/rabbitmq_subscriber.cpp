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
rabbitmq_subscriber_t::destroy_s()
{
	SYNCHRONIZED(mtx);

	destroy();
}

void
rabbitmq_subscriber_t::destroy()
{
	if (destroyed)
		return;

	destroyed = true;

	shutdown = true;
	
	if (rmq_poll_thread)
		rmq_poll_thread->join();
	
	delete rmq_poll_thread;

	rabbitmq_base_t::destroy();

	if (rmq_selector)
		rmq_selector->release();

	if (rmq_queue)
		rmq_queue->release();

	if (meta_cb)
		this->queue->enqueue(std::bind(meta_cb,PLUGIN_DESTROYED, nullptr));
}

bricks_error_code_e
rabbitmq_subscriber_t::init(cb_queue_t* queue, topic_cb_t msg_cb, const xtree_t* options)
{
	SYNCHRONIZED(mtx);

	ASSERT_PREINIT;

	this->msg_cb = msg_cb;

	this->rmq_queue = create_callback_queue();

	this->rmq_selector = create_selector();

	rmq_selector->init(rmq_queue);


	bool declare = true;
	bricks_error_code_e err = BRICKS_SUCCESS;

	do {

		name = get_opt<string>(options->get_property_value("/bricks/rabbitmq/subscriber", "name"));

		if (BRICKS_SUCCESS != (err = handle_connect_options("/bricks/rabbitmq/subscriber/methods/init/connection", queue, options)))
		{
			break;
		}

		if (BRICKS_SUCCESS != (err = handle_queue_declare_options("/bricks/rabbitmq/subscriber/methods/init/queue", this->default_queue_name, options)))
		{
			break;
		}

		rmq_poll_thread = new thread(&rabbitmq_subscriber_t::rmq_poll_loop, this);

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

	ASSERT_READY;

	bricks_promise_t p;
	auto f = p.get_future();

	rmq_queue->enqueue(std::bind(&rabbitmq_subscriber_t::do_subscribe, this, std::ref(p), topic, options));
	f.wait();

	return f.get();
}

void
rabbitmq_subscriber_t::do_subscribe(std::promise<bricks_error_code_e>& p, const string& topic, const xtree_t* options)
{
	bricks_error_code_e err = BRICKS_SUCCESS;

	do {
		err = handle_exchange_declare_options("/bricks/rabbitmq/subscriber/methods/subscribe/exchange", topic, options);
		if (err != BRICKS_SUCCESS)
			break;

		err = handle_queue_declare_options("/bricks/rabbitmq/subscriber/methods/subscribe/queue", this->default_queue_name, options);
		if (err != BRICKS_SUCCESS)
			break;

		err = handle_bind_options("/bricks/rabbitmq/subscriber/methods/subscribe/bind", this->default_queue_name, topic, options);
		if (err != BRICKS_SUCCESS)
			break;

	} while (false);

	p.set_value(err);
}

bricks_error_code_e
rabbitmq_subscriber_t::unsubscribe(const string& topic, const xtree_t* options)
{
	SYNCHRONIZED(mtx);

	ASSERT_READY;

	bricks_promise_t p;
	auto f = p.get_future();

	rmq_queue->enqueue(std::bind(&rabbitmq_subscriber_t::do_unsubscribe1, this, std::ref(p), topic, options));
	f.wait();

	return f.get();
}

void
rabbitmq_subscriber_t::do_unsubscribe1(std::promise<bricks_error_code_e>& p, const string& topic, const xtree_t* options)
{
	bricks_error_code_e err = BRICKS_SUCCESS;

	err = handle_unbind_options("/bricks/rabbitmq/subscriber/methods/unsubscribe/queue", this->default_queue_name, topic, options);

	p.set_value(err);
}

bricks_error_code_e
rabbitmq_subscriber_t::unsubscribe(const xtree_t* options)
{
	SYNCHRONIZED(mtx);

	ASSERT_READY;

	bricks_promise_t p;
	auto f = p.get_future();

	rmq_queue->enqueue(std::bind(&rabbitmq_subscriber_t::do_unsubscribe2, this, std::ref(p), options));
	f.wait();

	return f.get();
}

void  
rabbitmq_subscriber_t::do_unsubscribe2(std::promise<bricks_error_code_e>& p, const xtree_t* options)
{
	bricks_error_code_e err = BRICKS_SUCCESS;
	
	err = handle_unbind_options("/bricks/rabbitmq/subscriber/methods/unsubscribe/queue", this->default_queue_name, "*", options);

	p.set_value(err);
}

bricks_error_code_e 
rabbitmq_subscriber_t::start()
{
	SYNCHRONIZED(mtx);

	ASSERT_READY;

	return BRICKS_SUCCESS;
}

void 
rabbitmq_subscriber_t::set_meta_cb(meta_cb_t meta_cb)
{
	SYNCHRONIZED(mtx);

	this->meta_cb = meta_cb;
}

void
rabbitmq_subscriber_t::rmq_poll_loop()
{

	amqp_frame_t frame;

	bool amqp_error = false;

	timeval t;
	t.tv_sec = 0;
	t.tv_usec = 500000;

	for (;!shutdown && !amqp_error;) {

		while (rmq_selector->poll(0) == BRICKS_SUCCESS);

		amqp_rpc_reply_t ret;
		amqp_envelope_t envelope;

		amqp_maybe_release_buffers(amqp_conn);
		ret = amqp_consume_message(amqp_conn, &envelope, &t, 0);
		
		if (AMQP_RESPONSE_NORMAL != ret.reply_type) {
			if (AMQP_RESPONSE_LIBRARY_EXCEPTION == ret.reply_type &&
				AMQP_STATUS_UNEXPECTED_STATE == ret.library_error) {
				if (AMQP_STATUS_OK != amqp_simple_wait_frame(amqp_conn, &frame)) {
					amqp_error = true;
					log1(BRICKS_ALARM, "%s %%%%%% Failure reading frame.\n", this->name.c_str());
					continue;
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
							amqp_error = true;
							log1(BRICKS_ALARM, "%s %%%%%% Failure reading message.\n", this->name.c_str());
							continue;
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
						amqp_error = true;
						log1(BRICKS_ALARM, "%s %%%%%% Channel closed.\n", this->name.c_str());
						continue;

					case AMQP_CONNECTION_CLOSE_METHOD:
						/* a connection.close method happens when a connection exception
						 * occurs, this can happen by trying to use a channel that isn't
						 * open for example.
						 *
						 * In this case the whole connection must be restarted.
						 */
						amqp_error = true;
						log1(BRICKS_ALARM, "%s %%%%%% Connection closed.\n", this->name.c_str());
						continue;

					default:
						amqp_error = true;
						log1(BRICKS_ALARM, "%s %%%%%% An unexpected method was received %u\n", this->name.c_str(),
							frame.payload.method.id);
						continue;
					}
				}
			}

		}
		else {

			auto xt = create_xtree_from_xml(
				"<bricks>                " 
				"  <event last=\"true\"/>" 
				"</bricks>");

			this->queue->enqueue(
				std::bind(
					msg_cb,
					string((char *)envelope.exchange.bytes, envelope.exchange.len),
					create_buffer((const char*)envelope.exchange.bytes, (int)envelope.exchange.len),
					xt));

			amqp_destroy_envelope(&envelope);
		}

	}

	if (amqp_error)
		this->queue->enqueue(std::bind(&rabbitmq_subscriber_t::destroy_s, this));
}
