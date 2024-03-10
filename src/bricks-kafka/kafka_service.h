#pragma once

using namespace std;

class kafka_service_t : public service_t
{
public:

	kafka_service_t();

	virtual bricks_error_code_e init(const xtree_t* options) override;

	virtual bricks_error_code_e register_publisher(const string& topic, const xtree_t* options) override;

	virtual bricks_error_code_e unregister_publisher(const string& topic) override;
	
	virtual bricks_error_code_e publish(const string& topic, const buffer_t& buf, void* opaque, const xtree_t* options) override;

	virtual bricks_error_code_e subscribe(const string& topic, void* opaque, const xtree_t* options) override;

	virtual bricks_error_code_e unsubscribe(const string& topic) override;

	virtual bricks_error_code_e  poll() override;

private:

	bricks_error_code_e init_conf(rd_kafka_conf_t* conf, const xtree_t* options);

	virtual ~kafka_service_t();

	virtual void destroy();

	static void msg_delivered1(rd_kafka_t* rk,
		const rd_kafka_message_t* rkmessage,
		void* opaque);

	void msg_delivered(rd_kafka_t* rk,
		const rd_kafka_message_t* rkmessage,
		void* opaque);

	char errstr[512] = { '\0' };

	bool initiated;

	struct producer_t
	{
		rd_kafka_conf_t* rd_conf_h;

		rd_kafka_topic_t* rd_topic_h;

		rd_kafka_t* rd_producer_h;
	};

	void destroy_producer(producer_t &c);

	struct consumer_t
	{
		rd_kafka_conf_t* rd_conf_h;

		rd_kafka_topic_partition_list_t* rd_part_list_h;

		rd_kafka_t* rd_consumer_h;
	};

	void destroy_consumer(consumer_t& c);

	std::map<string, producer_t> producers;

	std::map<string, consumer_t> consumers;
};



