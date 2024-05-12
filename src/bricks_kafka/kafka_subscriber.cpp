#include "pch.h"
#include "kafka_subscriber.h"

using namespace std::placeholders;
using namespace bricks;
using namespace bricks::plugins;

msg_cb_t msg_cb = nullptr;

kafka_subscriber_t::kafka_subscriber_t()
{

}

kafka_subscriber_t::~kafka_subscriber_t()
{
	destroy();
}


bricks_error_code_e 
kafka_subscriber_t::init(cb_queue_t* queue, msg_cb_t msg_cb, const xtree_t* options)
{
	ASSERT_NOT_INITIATED;
	ASSERT_NOT_STARTED;

	bricks_error_code_e err = BRICKS_SUCCESS;

	this->msg_cb = msg_cb;

	this->cb_queue = queue;

	rd_conf_h = rd_kafka_conf_new();

	rd_kafka_conf_set_opaque(rd_conf_h, this);

	if (options)
	{
		auto enabled = get_opt<bool>(options->get_property_value("/bricks/rdkafka/rdlog", "enabled"));
		if (enabled)
		{
			rd_kafka_conf_set_log_cb(rd_conf_h, kafka_service_t::rd_log);
		}

		this->name = get_opt<string>(options->get_property_value("/bricks/rdkafka/consumer", "name"), "kafka-consumer");
	}

	if ((err = init_conf(rd_conf_h, options, "/bricks/rdkafka/consumer/configuration")) != BRICKS_SUCCESS)
	{
		destroy();
		return err;
	}

	rd_part_list_h = rd_kafka_topic_partition_list_new(1);
	
	// Create Kafka consumer instance
	rd_kafka_h = rd_kafka_new(RD_KAFKA_CONSUMER, rd_conf_h, NULL, 0);
	if (!rd_kafka_h)
	{
		log1(BRICKS_ALARM, "%s %%%%%% Failed to create rdkafka consumer(%d): %s.\n", this->name.c_str(), rd_kafka_last_error(), rd_kafka_err2str(rd_kafka_last_error()));
		destroy();
		return BRICKS_3RD_PARTY_ERROR;
	}

	rd_kafka_poll_set_consumer(rd_kafka_h);
	
	initiated = true;

	return BRICKS_SUCCESS;
}

bricks_error_code_e 
kafka_subscriber_t::start()
{
	ASSERT_INITIATED;
	ASSERT_NOT_STARTED;

	bricks_error_code_e err = BRICKS_SUCCESS;

	auto rd_err = rd_kafka_subscribe(rd_kafka_h, rd_part_list_h);

	if (rd_err) {
		log1(BRICKS_ALARM, "%s %%%%%% Failed to subscribe to topic(%d): %s.\n", this->name.c_str(), rd_err, rd_kafka_err2str(rd_err));
		destroy();
		return BRICKS_3RD_PARTY_ERROR;
	}

	if (start_rd_poll_loop() != BRICKS_SUCCESS)
	{
		destroy();
		return BRICKS_FAILURE_GENERIC;
	}

	started = true;

	return BRICKS_SUCCESS;
}

void
kafka_subscriber_t::destroy()
{
	initiated = false;
	started = false;

	stop_rd_poll_loop();

	if (rd_kafka_h)
	{
		rd_kafka_consumer_close(rd_kafka_h);
	}
		
	if (rd_part_list_h)
	{
		rd_kafka_topic_partition_list_destroy(rd_part_list_h);
		rd_part_list_h = nullptr;
	}
	
	if (rd_kafka_h)
	{
		rd_kafka_destroy(rd_kafka_h);
		rd_kafka_h = nullptr;
	}
	
}

bricks_error_code_e  
kafka_subscriber_t::register_topic(const string& topic, const xtree_t* options)
{
	ASSERT_INITIATED;
	ASSERT_NOT_STARTED;

	bricks_error_code_e err = BRICKS_SUCCESS;

	int partition = 0;

	if (options)
	{
		partition = get_opt<int>(options->get_property_value("/bricks/rdkafka/consumer/topic/partition", "value").value(),0);
	}

	rd_kafka_topic_partition_list_add(
		rd_part_list_h, 
		topic.c_str(),
		partition);

	return BRICKS_SUCCESS;

}

bricks_error_code_e
kafka_subscriber_t::rd_poll(int milliseconds, bool last_call)
{
	bricks_error_code_e  err = BRICKS_SUCCESS;

	if (last_call)
		return BRICKS_SUCCESS;

	rd_kafka_message_t* msg = rd_kafka_consumer_poll(rd_kafka_h, (int)milliseconds); // Poll for messages

	if (!msg)
		return BRICKS_TIMEOUT;

	if (msg->err)
	{
		log1(BRICKS_ALARM, "%s %%%%%% Error consuming rdkafka messages(%d): %s.",  this->name.c_str(), msg->err, rd_kafka_err2str(msg->err));
		started = false;
		return BRICKS_3RD_PARTY_ERROR;
	}
	else
	{
		auto xtree = create_xtree();

		if (cb_queue)
		{
			cb_queue->enqueue(std::bind(msg_cb, "", create_buffer((const char*)msg->payload, (int)msg->len), xtree));
		}
		else
		{
			try
			{
				msg_cb("", create_buffer((const char*)msg->payload, (int)msg->len), xtree);
			}
			catch (std::exception&) {}
		}
	
	}

	rd_kafka_message_destroy(msg);

	return err;
}



