#pragma once

using namespace std;

class kafka_wrapper_t
{
public:

	kafka_wrapper_t();

	bricks_error_code_e init(bricks_handle_t conf_h, msg_delivered_cb_t cb);

	bricks_error_code_e register_topic(const char *topic);

	bricks_error_code_e unregister_topic(const char* topic);

	bricks_error_code_e publish(const char* topic, bricks_handle_t xtree);

	virtual ~kafka_wrapper_t();

	virtual void destroy();

	static void msg_delivered1(rd_kafka_t* rk,
		const rd_kafka_message_t* rkmessage,
		void* opaque);

	void msg_delivered(rd_kafka_t* rk,
		const rd_kafka_message_t* rkmessage,
		void* opaque);

private:

	rd_kafka_conf_t* rd_conf_h;

	rd_kafka_t *rd_producer_h;

	msg_delivered_cb_t msg_delivered_cb;

	char errstr[512] = { '\0' };

	bool initiated;

	struct topic_t
	{
		rd_kafka_topic_conf_t* rd_topic_conf;

		rd_kafka_topic_t* rd_topic_h;
	};

	std::map<string, topic_t> topics;
};

