#pragma once

using namespace std;

class kafka_publisher_t : public publisher_service_t
{
public:

	kafka_publisher_t();

	virtual bricks_error_code_e init(const xtree_t* options) override;

	virtual bricks_error_code_e register_topic(const string& topic, const xtree_t* options) override;

	virtual bricks_error_code_e publish(const string& topic, const buffer_t& buf, void* opaque, const xtree_t* options) override;

	virtual bricks_error_code_e poll() override;

private:


	virtual ~kafka_publisher_t();

	virtual void destroy();

	static void msg_delivered1(rd_kafka_t* rk,
		const rd_kafka_message_t* rkmessage,
		void* opaque);

	void msg_delivered(rd_kafka_t* rk,
		const rd_kafka_message_t* rkmessage,
		void* opaque);

	char errstr[512] = { '\0' };

	bool initiated;

	rd_kafka_conf_t* rd_conf_h;

	rd_kafka_t* rd_producer_h;

	rd_kafka_conf_t* rd_conf_h = nullptr;

	struct producer_t
	{
	
		rd_kafka_topic_t* rd_topic_h;
	};

	void destroy_topic(producer_t& c);

	std::map<string, producer_t> producers;

	std::map<string, consumer_t> consumers;


};

