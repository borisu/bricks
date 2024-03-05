#include "pch.h"
#include "kafka_wrapper.h"

using namespace std::placeholders;
 

// https://github.com/confluentinc/librdkafka/blob/master/examples/rdkafka_example.c
static void
kafka_logger(const rd_kafka_t* rk, int level, const char* fac, const char* buf) {

	bricks_debug_level_e log_level = BRICKS_DEBUG;

	switch (level)
	{
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	case 6: { log_level = BRICKS_INFO; break; }
	default: { log_level = BRICKS_DEBUG; break; };
	};


	log1(log_level, "RDKAFKA-%i-%s: %s: %s\n"
		, fac,
		rk ? rd_kafka_name(rk) : NULL, buf);
}

msg_delivered_cb_t msg_cb = nullptr;

rd_kafka_t* producer = nullptr;

kafka_wrapper_t::kafka_wrapper_t():
	rd_conf_h(nullptr), 
	msg_delivered_cb(nullptr),
	initiated(false)
{

}

kafka_wrapper_t::~kafka_wrapper_t()
{

}

void 
kafka_wrapper_t::msg_delivered1(rd_kafka_t* rk,
	const rd_kafka_message_t* rkmessage,
	void* opaque) {

	((kafka_wrapper_t*)opaque)->msg_delivered(rk, rkmessage, opaque);
}

void kafka_wrapper_t::msg_delivered(rd_kafka_t* rk,
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

bricks_error_code_e 
kafka_wrapper_t::init(bricks_handle_t conf_h, msg_delivered_cb_t cb)
{
	bricks_error_code_e err = BRICKS_SUCCESS;

	do {

		rd_conf_h = rd_kafka_conf_new();
		msg_delivered_cb = cb;

		int c = 0;
		c = bricks_get_children_count(conf_h, "/configuration");
		for (int i = 0; i < c && err == BRICKS_SUCCESS; i++)
		{

			if (strcmp(bricks_get_child_name_by_index(conf_h, "/configuration", i), "property") != 0)
				continue;

			auto name = bricks_get_child_property_by_index(conf_h, "/configuration", i, "name");
			auto value = bricks_get_child_property_by_index(conf_h, "/configuration", i, "value");

			char* errstr = nullptr;
			if (rd_kafka_conf_set(rd_conf_h, name, value,
				errstr, sizeof(errstr)) !=
				RD_KAFKA_CONF_OK) {
				log1(BRICKS_ALARM, "%% %s\n", errstr);
				err = BRICKS_INVALID_PARAM;
				break;
			}
		}


		/* Set logger */
		rd_kafka_conf_set_log_cb(rd_conf_h, kafka_logger);

		/* Set up a message delivery report callback.
		* It will be called once for each message, either on successful
		* delivery to broker, or upon failure to deliver to broker. */
		rd_kafka_conf_set_opaque(rd_conf_h, this);
	
		rd_kafka_conf_set_dr_msg_cb(rd_conf_h, msg_delivered1);

		// Create Kafka producer instance
		rd_producer_h = rd_kafka_new(RD_KAFKA_PRODUCER, rd_conf_h, NULL, 0);
		if (!rd_producer_h) {
			log1(BRICKS_ALARM, "Failed to create producer: %s\n", rd_kafka_err2str(rd_kafka_last_error()));
			err = BRICKS_INVALID_PARAM;
			break;
		}

	}  while (false);

	if (err != BRICKS_SUCCESS)
		destroy();

	return err;
	
}

bricks_error_code_e 
kafka_wrapper_t::publish(const char* topic, bricks_handle_t xtree)
{ 
	return BRICKS_SUCCESS;
}


void
kafka_wrapper_t::destroy()
{
	if (rd_producer_h)
	{
		rd_kafka_flush(rd_producer_h, 5000);
		rd_kafka_destroy(rd_producer_h);
	}

	if (rd_conf_h)
	{
		rd_kafka_conf_destroy(rd_conf_h);
		rd_conf_h = nullptr;
	}
	

	if (msg_cb)
	{
		msg_cb = nullptr;
	}

}


bricks_error_code_e 
kafka_wrapper_t::unregister_topic(const char* topic)
{
	return BRICKS_SUCCESS;
}

bricks_error_code_e 
kafka_wrapper_t::register_topic(const char* topic)
{
	if (topics.count(topic) > 0)
		return BRICKS_INVALID_PARAM;

	rd_kafka_topic_conf_t* rd_topic_conf_h = 
		rd_kafka_topic_conf_new();

	/* Create topic */
	rd_kafka_topic_t *rd_topic_h = 
		rd_kafka_topic_new(rd_producer_h, topic, rd_topic_conf_h);

	topics[topic] = topic_t { rd_topic_conf_h ,rd_topic_h };

	return BRICKS_SUCCESS;

}

