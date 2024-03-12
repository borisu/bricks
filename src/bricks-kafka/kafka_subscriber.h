#pragma once
#include "kafka_service.h"
#include "services_api.h"

using namespace std;

class kafka_subscriber_t : 
	public kafka_service_t,
	public subscriber_service_t
{
public:

	kafka_subscriber_t();

	virtual bricks_error_code_e init(msg_cb_t msg_cb, const xtree_t* options) override;

	virtual bricks_error_code_e register_topic(const string& topic, const xtree_t* options) override;

	virtual bricks_error_code_e subscribe(void* opaque, const xtree_t* options ) override;

	virtual bricks_error_code_e poll(size_t timeout) override;

private:

	virtual ~kafka_subscriber_t();

	virtual void destroy();
	
	rd_kafka_conf_t* rd_conf_h = nullptr;

	rd_kafka_t* rd_kafka_h = nullptr;

	rd_kafka_topic_partition_list_t* rd_part_list_h = nullptr;

	msg_cb_t msg_cb;

	void* opaque = nullptr;
	
};



