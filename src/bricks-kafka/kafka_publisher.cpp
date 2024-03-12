#include "pch.h"
#include "kafka_publisher.h"


bricks_error_code_e
kafka_subscriber_t::publish(const string& topic, const buffer_t& buf, void* opaque, const xtree_t* options)
{
	auto it = producers.find(topic);
	if (it == producers.end())
	{
		return BRICKS_INVALID_PARAM;
	}

	producer_t& p = it->second;

	auto err = rd_kafka_producev(
		/* Producer handle */
		p.rd_producer_h,
		/* Topic name */
		RD_KAFKA_V_TOPIC(topic.c_str()),
		/* Make a copy of the payload. */
		RD_KAFKA_V_MSGFLAGS(RD_KAFKA_MSG_F_COPY),
		/* Message value and length */
		RD_KAFKA_V_VALUE(&buf[0], buf.size()),
		/* Per-Message opaque, provided in
		 * delivery report callback as
		 * msg_opaque. */
		RD_KAFKA_V_OPAQUE(opaque),
		/* End sentinel */
		RD_KAFKA_V_END);

	if (err != RD_KAFKA_RESP_ERR_NO_ERROR)
	{
		log1(BRICKS_DEBUG, "Failed to produce message : %s\n", rd_kafka_err2str(rd_kafka_last_error()));
		return BRICKS_3RD_PARTY_ERROR;
	}

	return BRICKS_SUCCESS;

}

void
kafka_subscriber_t::destroy_producer(producer_t& p)
{
	if (p.rd_topic_h)
	{
		rd_kafka_topic_destroy(p.rd_topic_h);
		p.rd_topic_h = nullptr;
	}

	if (p.rd_producer_h)
	{
		rd_kafka_destroy(p.rd_producer_h);
		p.rd_producer_h = nullptr;
	}

	if (p.rd_conf_h)
	{
		rd_kafka_conf_destroy(p.rd_conf_h);
		p.rd_conf_h = nullptr;
	}
}

bricks_error_code_e
kafka_subscriber_t::register_topic(const string& topic, const xtree_t* options)
{
	bricks_error_code_e err = BRICKS_SUCCESS;

	auto it = producers.find(topic);
	if (it != producers.end())
	{
		return BRICKS_INVALID_STATE;
	}

	producer_t p;
	memset(&p, 0, sizeof(p));





	// Create producer instance
	p.rd_producer_h = rd_kafka_new(RD_KAFKA_PRODUCER, p.rd_conf_h, NULL, 0);
	if (!p.rd_producer_h) {
		log1(BRICKS_ALARM, "Failed to create producer: %s\n", rd_kafka_err2str(rd_kafka_last_error()));
		destroy_producer(p);
		return BRICKS_3RD_PARTY_ERROR;
	}

	/* Create topic */
	p.rd_topic_h = rd_kafka_topic_new(p.rd_producer_h, topic.c_str(), nullptr);
	if (!p.rd_topic_h) {
		log1(BRICKS_ALARM, "Failed to create topic: %s\n", rd_kafka_err2str(rd_kafka_last_error()));
		destroy_producer(p);
		return BRICKS_3RD_PARTY_ERROR;
	}

	producers[topic] = p;
	return BRICKS_SUCCESS;
}

bricks_error_code_e
kafka_subscriber_t::unregister_publisher(const string& topic)
{
	auto it = producers.find(topic);
	if (it == producers.end())
	{
		return BRICKS_INVALID_STATE;
	}

	destroy_producer(it->second);
	producers.erase(it);

	return BRICKS_SUCCESS;
}
