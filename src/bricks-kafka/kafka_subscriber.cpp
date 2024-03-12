#include "pch.h"
#include "kafka_subscriber.h"

using namespace std::placeholders;
 

msg_cb_t msg_cb = nullptr;

kafka_subscriber_t::kafka_subscriber_t()
{

}

kafka_subscriber_t::~kafka_subscriber_t()
{

}

void 
kafka_subscriber_t::msg_delivered1(rd_kafka_t* rk,
	const rd_kafka_message_t* rkmessage,
	void* opaque) {

	((kafka_subscriber_t*)opaque)->msg_delivered(rk, rkmessage, opaque);
}

void 
kafka_subscriber_t::msg_delivered(rd_kafka_t* rk,
	const rd_kafka_message_t* rkmessage,
	void* opaque) {
	if (rkmessage->err)
		log1(BRICKS_DEBUG,
			"%% Message delivery failed (broker %" PRId32 "): %s\n",
			rd_kafka_message_broker_id(rkmessage),	
			rd_kafka_err2str(rkmessage->err));
	else
		log1(BRICKS_DEBUG,
			"%% Message delivered (%zd bytes, offset %" PRId64
			", "
			"partition %" PRId32 ", broker %" PRId32 "): %.*s\n",
			rkmessage->len, rkmessage->offset, rkmessage->partition,
			rd_kafka_message_broker_id(rkmessage),
			(int)rkmessage->len, (const char*)rkmessage->payload);

}

bricks_error_code_e kafka_subscriber_t::init(const xtree_t* options)
{
	ASSERT_NOT_INITIATED;

	bricks_error_code_e err = BRICKS_SUCCESS;

	rd_conf_h = rd_kafka_conf_new();

	rd_kafka_conf_set_log_cb(rd_conf_h, kafka_service_t::kafka_logger);

	if ((err = init_conf(rd_conf_h, options)) != BRICKS_SUCCESS)
	{
		destroy();
		return err;
	}

	// Create Kafka consumer instance
	rd_kafka_h = rd_kafka_new(RD_KAFKA_CONSUMER, rd_conf_h, NULL, 0);
	if (!rd_kafka_h)
	{
		log1(BRICKS_ALARM, "Failed to create Kafka consumer: %s\n", rd_kafka_err2str(rd_kafka_last_error()));
		destroy();
		return BRICKS_3RD_PARTY_ERROR;
	}

	rd_part_list_h = rd_kafka_topic_partition_list_new(1);
	if (!rd_part_list_h)
	{
		log1(BRICKS_ALARM, "Failed to create Kafka consumer: %s\n", rd_kafka_err2str(rd_kafka_last_error()));
		destroy();
		return BRICKS_3RD_PARTY_ERROR;
	}

	initiated = true;

	return BRICKS_SUCCESS;
}



void
kafka_subscriber_t::destroy()
{
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

	auto rd_err = rd_kafka_subscribe(rd_kafka_h, rd_part_list_h);

	if (rd_err) {
		log1(BRICKS_ALARM, "Failed to subscribe to topic: %s\n", rd_kafka_err2str(rd_err));
		destroy();
		return BRICKS_3RD_PARTY_ERROR;
	}

	

	return BRICKS_SUCCESS;

}


