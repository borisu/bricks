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

void
kafka_subscriber_t::name(const char* pname)
{
	bname = pname;
};

const char*
kafka_subscriber_t::name() const
{
	return bname.c_str();
}

bricks_error_code_e 
kafka_subscriber_t::init(cb_queue_t* queue, msg_cb_t msg_cb, const xtree_t* options)
{
	ASSERT_NOT_INITIATED;

	bricks_error_code_e err = BRICKS_SUCCESS;

	this->msg_cb = msg_cb;

	this->cb_queue = queue;

	rd_conf_h = rd_kafka_conf_new();

	rd_kafka_conf_set_log_cb(rd_conf_h, kafka_service_t::rd_log);

	if ((err = init_conf(rd_conf_h, options)) != BRICKS_SUCCESS)
	{
		destroy();
		return err;
	}

	rd_part_list_h = rd_kafka_topic_partition_list_new(1);
	if (!rd_part_list_h)
	{
		log1(BRICKS_ALARM, "Failed to create Kafka consumer topics list: %s\n", rd_kafka_err2str(rd_kafka_last_error()));
		destroy();
		return BRICKS_3RD_PARTY_ERROR;
	}

	// Create Kafka consumer instance
	rd_kafka_h = rd_kafka_new(RD_KAFKA_CONSUMER, rd_conf_h, NULL, 0);
	if (!rd_kafka_h)
	{
		log1(BRICKS_ALARM, "Failed to create Kafka consumer: %s\n", rd_kafka_err2str(rd_kafka_last_error()));
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

	this->opaque = opaque;

	auto rd_err = rd_kafka_subscribe(rd_kafka_h, rd_part_list_h);

	if (rd_err) {
		log1(BRICKS_ALARM, "Failed to subscribe to topic: %s\n", rd_kafka_err2str(rd_err));
		return BRICKS_3RD_PARTY_ERROR;
	}

	if (start_rd_poll_loop() != BRICKS_SUCCESS)
	{
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

	rd_kafka_topic_partition_list_add(rd_part_list_h, topic.c_str(),
		/* the partition is ignored
		 * by subscribe() */
		0);

	return BRICKS_SUCCESS;

}

bricks_error_code_e
kafka_subscriber_t::rd_poll(int milliseconds, bool last_call)
{
	bricks_error_code_e  err = BRICKS_SUCCESS;

	rd_kafka_message_t* msg = rd_kafka_consumer_poll(rd_kafka_h, (int)milliseconds); // Poll for messages

	if (!msg)
		return BRICKS_TIMEOUT;

	if (msg->err)
	{
		log1(BRICKS_ALARM, "error consuming messages:%s", rd_kafka_err2str(rd_kafka_last_error()));
		return BRICKS_3RD_PARTY_ERROR;
	}
	else
	{
		auto xtree = create_xtree();
		
		cb_queue->enqueue(std::bind(msg_cb, this->opaque, create_buffer((const char*)msg->payload, (int)msg->len), xtree));
		
	}

	rd_kafka_message_destroy(msg);

	return err;
}



