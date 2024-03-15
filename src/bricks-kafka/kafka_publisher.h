#pragma once
#include "kafka_service.h"

using namespace std;

class kafka_publisher_t : 
	public kafka_service_t,
	public publisher_service_t
{
public:

	kafka_publisher_t();

	virtual bricks_error_code_e init(delivery_cb_t msg_cb, const xtree_t* options) override;

	virtual bricks_error_code_e register_topic(const string& topic, const xtree_t* options) override;

	virtual bricks_error_code_e publish(const string& topic, const buffer_t& buf, void* opaque, const xtree_t* options)  override;

	virtual bricks_error_code_e poll(size_t timeout) override;

	virtual ~kafka_publisher_t();

private:

	virtual void destroy();

	static void msg_delivered1(rd_kafka_t* rk,
		const rd_kafka_message_t* rkmessage,
		void* opaque);

	void msg_delivered(rd_kafka_t* rk,
		const rd_kafka_message_t* rkmessage,
		void* opaque);

	char errstr[512] = { '\0' };

	bool initiated;

	rd_kafka_conf_t* rd_conf_h = nullptr;

	rd_kafka_t* rd_producer_h = nullptr;

	map<string,rd_kafka_topic_t*> rd_topics;
	
	delivery_cb_t msg_cb;


};

