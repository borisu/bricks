#include "pch.h"
#include "kafka_service.h"

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

msg_cb_t msg_cb = nullptr;

kafka_service_t::kafka_service_t():
	initiated(false)
{

}

kafka_service_t::~kafka_service_t()
{

}

void 
kafka_service_t::msg_delivered1(rd_kafka_t* rk,
	const rd_kafka_message_t* rkmessage,
	void* opaque) {

	((kafka_service_t*)opaque)->msg_delivered(rk, rkmessage, opaque);
}

void 
kafka_service_t::msg_delivered(rd_kafka_t* rk,
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

void 
kafka_service_t::destroy_producer(producer_t& p)
{
	if (p.rd_topic_h)
	{
		rd_kafka_topic_destroy(p.rd_topic_h);
		p.rd_topic_h= nullptr;
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
kafka_service_t::register_publisher(const string& topic, const xtree_t* options)
{
	bricks_error_code_e err = BRICKS_SUCCESS;

	auto it = producers.find(topic);
	if (it != producers.end())
	{
		return BRICKS_INVALID_STATE;
	}

	producer_t p;
	memset(&p, 0, sizeof(p));

	p.rd_conf_h = rd_kafka_conf_new();

	if ((err = init_conf(p.rd_conf_h, options)) != BRICKS_SUCCESS)
	{
		destroy_producer(p);
		return err;
	}

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
kafka_service_t::unregister_publisher(const string& topic)
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


bricks_error_code_e  
kafka_service_t::unsubscribe(const string& topic)
{
	auto it = consumers.find(topic);
	if (it == consumers.end())
	{
		return BRICKS_INVALID_STATE;
	}

	destroy_consumer(it->second);
	consumers.erase(it);

	return BRICKS_SUCCESS;

}

void
kafka_service_t::destroy_consumer(consumer_t& c)
{
	if (c.rd_part_list_h)
	{
		rd_kafka_topic_partition_list_destroy(c.rd_part_list_h);
		c.rd_part_list_h = nullptr;
	}
	
	if (c.rd_consumer_h)
	{
		rd_kafka_destroy(c.rd_consumer_h);
		c.rd_consumer_h = nullptr;
	}

	if (c.rd_conf_h)
	{
		rd_kafka_conf_destroy(c.rd_conf_h);
		c.rd_conf_h = nullptr;
	}
}

bricks_error_code_e  
kafka_service_t::subscribe(const string& topic, void* opaque, const xtree_t* options)
{
	bricks_error_code_e err = BRICKS_SUCCESS;

	auto it = consumers.find(topic);
	if (it != consumers.end())
	{
		return BRICKS_INVALID_STATE;
	}

	consumer_t c;
	memset(&c, 0, sizeof(c));

	c.rd_conf_h = rd_kafka_conf_new();

	if ((err = init_conf(c.rd_conf_h, options)) != BRICKS_SUCCESS)
	{
		destroy_consumer(c);
		return err;
	}

	// Create Kafka consumer instance
	c.rd_consumer_h = rd_kafka_new(RD_KAFKA_CONSUMER,c.rd_conf_h, NULL, 0);
	if (!c.rd_consumer_h) 
	{
		log1(BRICKS_ALARM, "Failed to create Kafka consumer: %s\n", rd_kafka_err2str(rd_kafka_last_error()));
		destroy_consumer(c);
		return BRICKS_3RD_PARTY_ERROR;
	}

	c.rd_part_list_h = rd_kafka_topic_partition_list_new(1);
	rd_kafka_topic_partition_list_add(c.rd_part_list_h, topic.c_str(), RD_KAFKA_PARTITION_UA);
	auto rd_err = rd_kafka_subscribe(c.rd_consumer_h, c.rd_part_list_h);
	if (rd_err) {
		log1(BRICKS_ALARM, "Failed to subscribe to topic: %s\n", rd_kafka_err2str(rd_err));
		destroy_consumer(c);
		return BRICKS_3RD_PARTY_ERROR;
	}

	consumers[topic] = c;

	return BRICKS_SUCCESS;

}

bricks_error_code_e  
kafka_service_t::poll()
{
	return BRICKS_SUCCESS;

}

bricks_error_code_e 
kafka_service_t::init_conf(rd_kafka_conf_t* conf, const xtree_t* options)
{
	bricks_error_code_e err = BRICKS_SUCCESS;

	try
	{
		int c = options->get_node_children_count("/configuration").value();


		for (int i = 0; i < c && err == BRICKS_SUCCESS; i++)
		{
			auto prop = options->get_child_name_by_index("/configuration", i).value();
			if (prop != "property")
				continue;

			auto name = options->get_child_property_value_as_string("/configuration", i, "name").value();
			auto value = options->get_child_property_value_as_string("/configuration", i, "value").value();

			char* errstr = nullptr;
			if (rd_kafka_conf_set(conf, name.c_str(), value.c_str(), errstr, sizeof(errstr)) != RD_KAFKA_CONF_OK) 
			{
				log1(BRICKS_ALARM, "%% %s\n", errstr);
				err = BRICKS_3RD_PARTY_ERROR;
				break;
			}
		}
	}
	catch (std::bad_optional_access&)
	{
		err = BRICKS_INVALID_PARAM;
	}

	return err;

}

bricks_error_code_e 
kafka_service_t::init(const xtree_t *options)
{
	return BRICKS_SUCCESS;
	
}

bricks_error_code_e  
kafka_service_t::publish(const string& topic, const buffer_t& buf, void* opaque, const xtree_t *options)
{
	auto it = producers.find(topic);
	if (it == producers.end())
	{
		return BRICKS_INVALID_PARAM;
	}

	producer_t &p = it->second;

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
kafka_service_t::destroy()
{
	
}


