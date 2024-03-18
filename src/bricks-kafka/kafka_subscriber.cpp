#include "pch.h"
#include "kafka_subscriber.h"

using namespace std::placeholders;
 

msg_cb_t msg_cb = nullptr;

kafka_subscriber_t::kafka_subscriber_t()
{

}

kafka_subscriber_t::~kafka_subscriber_t()
{
	destroy();
}

bricks_error_code_e 
kafka_subscriber_t::init(msg_cb_t msg_cb, const xtree_t* options)
{
	ASSERT_NOT_INITIATED;

	bricks_error_code_e err = BRICKS_SUCCESS;

	this->msg_cb = msg_cb;

	rd_conf_h = rd_kafka_conf_new();

	rd_kafka_conf_set_log_cb(rd_conf_h, kafka_service_t::kafka_logger);

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



void
kafka_subscriber_t::destroy()
{
	initiated = false;

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

	if (rd_conf_h)
	{
		rd_kafka_conf_destroy(rd_conf_h);
		rd_conf_h = nullptr;
	}
}

bricks_error_code_e  
kafka_subscriber_t::register_topic(const string& topic, const xtree_t* options)
{
	ASSERT_INITIATED;

	bricks_error_code_e err = BRICKS_SUCCESS;

	rd_kafka_topic_partition_list_add(rd_part_list_h, topic.c_str(),
		/* the partition is ignored
		 * by subscribe() */
		0);

	return BRICKS_SUCCESS;

}


bricks_error_code_e
kafka_subscriber_t::subscribe(void* opaque,  const xtree_t* options)
{
	ASSERT_INITIATED;

	bricks_error_code_e err = BRICKS_SUCCESS;

	this->opaque = opaque;

	auto rd_err = rd_kafka_subscribe(rd_kafka_h, rd_part_list_h);

	if (rd_err) {
		log1(BRICKS_ALARM, "Failed to subscribe to topic: %s\n", rd_kafka_err2str(rd_err));
		destroy();
		return BRICKS_3RD_PARTY_ERROR;
	}

	return BRICKS_SUCCESS;

}

bricks_error_code_e 
kafka_subscriber_t::poll(size_t timeout)
{
	bricks_error_code_e  err = BRICKS_SUCCESS;

	rd_kafka_message_t* msg = rd_kafka_consumer_poll(rd_kafka_h, (int)timeout); // Poll for messages

	if (!msg)
		return BRICKS_TIMEOUT;

	if (msg->err)
	{
		return BRICKS_3RD_PARTY_ERROR;
	}
	else
	{
		auto xtree = create_xtree();

		msg_cb(this->opaque , (const char*)msg->payload, (size_t)msg->len, *xtree);

		destroy_xtree(xtree);
	}

	
	rd_kafka_message_destroy(msg);

	return err;
	

}

