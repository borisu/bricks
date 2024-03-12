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

	virtual bricks_error_code_e init(const xtree_t* options) override;

	virtual bricks_error_code_e register_topic(const string& topic, const xtree_t* options) = 0;

	virtual bricks_error_code_e subscribe(const string& topic, const xtree_t* options) = 0;

private:

	virtual ~kafka_subscriber_t();

	virtual void destroy();

	static void msg_delivered1(rd_kafka_t* rk,
		const rd_kafka_message_t* rkmessage,
		void* opaque);

	void msg_delivered(rd_kafka_t* rk,
		const rd_kafka_message_t* rkmessage,
		void* opaque);

	char errstr[512] = { '\0' };

	rd_kafka_conf_t* rd_conf_h;

	rd_kafka_t* rd_kafka_h;

	rd_kafka_topic_partition_list_t* rd_part_list_h;
	
};



