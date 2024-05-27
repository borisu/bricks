#include "pch.h"
#include "kafka_publisher.h"

using namespace bricks;
using namespace bricks::plugins;


kafka_publisher_t::kafka_publisher_t()
{

};

kafka_publisher_t::~kafka_publisher_t()
{
	destroy();
};

bricks_error_code_e 
kafka_publisher_t::init(cb_queue_t* queue, const xtree_t* options)
{
	SYNCHRONIZED(mtx);

	ASSERT_NOT_INITIATED;
	ASSERT_NOT_STARTED;

	bricks_error_code_e err = BRICKS_SUCCESS;

	this->cb_queue = queue;

	rd_conf_h = rd_kafka_conf_new();

	rd_kafka_conf_set_opaque(rd_conf_h, (void*)this);

	if (options)
	{
		auto enabled = get_opt<bool>(options->get_property_value("/bricks/rdkafka/rdlog", "enabled"));
		if (enabled)
		{
			rd_kafka_conf_set_log_cb(rd_conf_h, kafka_service_t::rd_log);
		}
	
		this->name = get_opt<string>(options->get_property_value("/bricks/rdkafka/producer", "name"), "kafka-producer");

	}

	if ((err = init_conf(rd_conf_h, options, "bricks/rdkafka/producer/configuration")) != BRICKS_SUCCESS)
	{
		destroy();
		return err;
	}

	
	// Create producer instance
	rd_producer_h = rd_kafka_new(RD_KAFKA_PRODUCER, rd_conf_h, NULL, 0);
	if (!rd_producer_h) {
		log1(BRICKS_ALARM, "%s %%%%%%% Error creating rdkafka producer (%d): %s.", this->name.c_str(), rd_kafka_last_error(), rd_kafka_err2str(rd_kafka_last_error()));
		destroy();
		return BRICKS_3RD_PARTY_ERROR;
	}

	initiated = true;

	return BRICKS_SUCCESS;
}

bricks_error_code_e 
kafka_publisher_t::start()
{
	SYNCHRONIZED(mtx);

	ASSERT_INITIATED;
	ASSERT_NOT_STARTED;

	
	bricks_error_code_e err = BRICKS_SUCCESS;

	if ((err = start_rd_poll_loop()) == BRICKS_SUCCESS)
	{
		started = true;
	}

	return err;
}

bricks_error_code_e
kafka_publisher_t::publish(const string& topic, const char* buf, size_t size, const xtree_t* options)
{
	SYNCHRONIZED(mtx);

	ASSERT_INITIATED;
	ASSERT_STARTED;
	
	auto it = rd_topics.find(topic);
	if (it == rd_topics.end())
	{
		return BRICKS_INVALID_PARAM;
	}

	int32_t partition = RD_KAFKA_PARTITION_UA;
	void*	key = NULL;
	size_t  key_size= 0;

	if (options)
	{
		auto key_opt = options->get_node_value("/bricks/rdkafka/producer/key");
		if (key_opt.has_value())
		{
			key = (void*) & (*key_opt.value())[0];
			key_size = key_opt.value()->size();
		}
	}

	auto err = rd_kafka_produce(
		it->second, 
		partition,
		RD_KAFKA_MSG_F_COPY,
		/* Payload and length */
		(void*)buf, size,
		/* Optional key and its length */
		key,
		key_size,
		/* Message opaque, provided in
		 * delivery report callback as
		 * msg_opaque. */
		NULL);

	if (err != RD_KAFKA_RESP_ERR_NO_ERROR)
	{
		log1(BRICKS_DEBUG, "%s %%%%%%% Error producing rdkafka message (%d): %s.", rd_kafka_last_error(), rd_kafka_err2str(rd_kafka_last_error()));
		return BRICKS_3RD_PARTY_ERROR;
	}

	return BRICKS_SUCCESS;
}

void
kafka_publisher_t::destroy()
{
	stop_rd_poll_loop();

	for (auto p : rd_topics)
	{
		rd_kafka_topic_destroy(p.second);
	}
	if (rd_producer_h)
	{
		rd_kafka_destroy(rd_producer_h);
		rd_producer_h = nullptr;
	}
}

bricks_error_code_e
kafka_publisher_t::add_topic(const string& topic, const xtree_t* options)
{
	SYNCHRONIZED(mtx);

	ASSERT_INITIATED;
	ASSERT_NOT_STARTED;

	bricks_error_code_e err = BRICKS_SUCCESS;

	auto it = rd_topics.find(topic);
	if (it != rd_topics.end())
	{
		return BRICKS_INVALID_STATE;
	}

	rd_kafka_topic_conf_t* topic_conf = nullptr; 
	

	if (options)
	{
		topic_conf = rd_kafka_topic_conf_new();
		if ((err = init_topic_conf(topic_conf, options, "/bricks/rdkafka/producer/topic/configuration")) != BRICKS_SUCCESS)
		{
			return err;
		}
	}


	/* Create topic */
	auto rd_topic_h = rd_kafka_topic_new(rd_producer_h, topic.c_str(), topic_conf);
	if (!rd_topic_h) {
		log1(BRICKS_ALARM, "%s %%%%%%% Error creating rdkafka topic(%d): %s\n", this->name.c_str(), rd_kafka_last_error(), rd_kafka_err2str(rd_kafka_last_error()));
		return BRICKS_3RD_PARTY_ERROR;
	}

	rd_topics[topic] = rd_topic_h;

	return BRICKS_SUCCESS;
}

bricks_error_code_e
kafka_publisher_t::rd_poll(int milliseconds, bool last_call)
{
	if (last_call)
	{
		rd_kafka_flush(rd_producer_h, milliseconds);
	}
	else
	{
		auto err = rd_kafka_poll(rd_producer_h, (int)milliseconds);
		if (err != RD_KAFKA_RESP_ERR_NO_ERROR)
		{
			log1(BRICKS_ALARM, "%s %%%%%%% Error polling rdkafka messages (%d): %s.", rd_kafka_last_error(), rd_kafka_err2str(rd_kafka_last_error()));
			return BRICKS_3RD_PARTY_ERROR;
		}

	}

	return BRICKS_SUCCESS;
}

bool 
kafka_publisher_t::check_capability(plugin_capabilities_e)
{
	return false;
}