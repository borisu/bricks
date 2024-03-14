#include "pch.h"
#include "kafka_publisher.h"

void
kafka_publisher_t::msg_delivered1(rd_kafka_t* rk,
	const rd_kafka_message_t* rkmessage,
	void* opaque) {

	((kafka_publisher_t*)opaque)->msg_delivered(rk, rkmessage, opaque);
}

void
kafka_publisher_t::msg_delivered(rd_kafka_t* rk,
	const rd_kafka_message_t* rkmessage,
	void* opaque) {
	if (rkmessage->err) 
	{
		log1(BRICKS_DEBUG,
			"%% Message delivery failed (broker %" PRId32 "): %s\n",
			rd_kafka_message_broker_id(rkmessage),
			rd_kafka_err2str(rkmessage->err));
	}
	else
	{
		log1(BRICKS_DEBUG,
			"%% Message delivered (%zd bytes, offset %" PRId64
			", "
			"partition %" PRId32 ", broker %" PRId32 "): %.*s\n",
			rkmessage->len, rkmessage->offset, rkmessage->partition,
			rd_kafka_message_broker_id(rkmessage),
			(int)rkmessage->len, (const char*)rkmessage->payload);
	}

}

kafka_publisher_t::kafka_publisher_t(){};

kafka_publisher_t::~kafka_publisher_t()
{
	destroy();
};

bricks_error_code_e 
kafka_publisher_t::init(delivery_cb_t msg_cb, const xtree_t* options)
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

	rd_kafka_conf_set_opaque(rd_conf_h, (void*)this);

	rd_kafka_conf_set_dr_msg_cb(rd_conf_h, msg_delivered1);

	// Create producer instance
	rd_producer_h = rd_kafka_new(RD_KAFKA_PRODUCER, rd_conf_h, NULL, 0);
	if (!rd_producer_h) {
		log1(BRICKS_ALARM, "Failed to create producer: %s\n", rd_kafka_err2str(rd_kafka_last_error()));
		destroy();
		return BRICKS_3RD_PARTY_ERROR;
	}

	initiated = true;

	return BRICKS_SUCCESS;
}


bricks_error_code_e
kafka_publisher_t::publish(const string& topic, const buffer_t& buf, void* opaque, const xtree_t* options)
{
	ASSERT_INITIATED;

	auto it = rd_topics.find(topic);
	if (it == rd_topics.end())
	{
		return BRICKS_INVALID_PARAM;
	}

	auto err = rd_kafka_produce(
		it->second, 
		RD_KAFKA_PARTITION_UA,
		RD_KAFKA_MSG_F_COPY,
		/* Payload and length */
		(void*) & buf[0], buf.size(),
		/* Optional key and its length */
		NULL, 0,
		/* Message opaque, provided in
		 * delivery report callback as
		 * msg_opaque. */
		NULL);

	if (err != RD_KAFKA_RESP_ERR_NO_ERROR)
	{
		log1(BRICKS_DEBUG, "Failed to produce message : %s\n", rd_kafka_err2str(rd_kafka_last_error()));
		return BRICKS_3RD_PARTY_ERROR;
	}

	return BRICKS_SUCCESS;

}

void
kafka_publisher_t::destroy()
{
	for (auto p : rd_topics)
	{
		rd_kafka_topic_destroy(p.second);
	}
	if (rd_producer_h)
	{
		rd_kafka_destroy(rd_producer_h);
		rd_producer_h = nullptr;
	}

	if (rd_conf_h)
	{
		rd_kafka_conf_destroy(rd_conf_h);
		rd_conf_h = nullptr;
	}
}

bricks_error_code_e
kafka_publisher_t::register_topic(const string& topic, const xtree_t* options)
{
	bricks_error_code_e err = BRICKS_SUCCESS;

	auto it = rd_topics.find(topic);
	if (it != rd_topics.end())
	{
		return BRICKS_INVALID_STATE;
	}

	/* Create topic */
	auto rd_topic_h = rd_kafka_topic_new(rd_producer_h, topic.c_str(), nullptr);
	if (!rd_topic_h) {
		log1(BRICKS_ALARM, "Failed to create topic: %s\n", rd_kafka_err2str(rd_kafka_last_error()));
		destroy();
		return BRICKS_3RD_PARTY_ERROR;
	}

	rd_topics[topic] = rd_topic_h;

	return BRICKS_SUCCESS;
}

bricks_error_code_e 
kafka_publisher_t::poll(size_t timeout)
{
	rd_kafka_poll(rd_producer_h, (int)timeout);
	return BRICKS_SUCCESS;
}