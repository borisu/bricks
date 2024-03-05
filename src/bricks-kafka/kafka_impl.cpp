#pragma once
#include "pch.h"
#include "bricks_api.h"
#include "logger_api.h"
#include "kafka_api.h"
#include "xtree_api.h"
#include "services_api.h"
#include "librdkafka/rdkafka.h"
#include "kafka_wrapper.h"


#ifdef __cplusplus
extern "C" {
#endif


bricks_handle_t
bricks_kafka_init(bricks_handle_t kafka_config_h, msg_delivered_cb_t kafka_msg_delivered_cb)
{

	kafka_wrapper_t * w  = new kafka_wrapper_t();
	if ((w->init(kafka_config_h, kafka_msg_delivered_cb)) != BRICKS_SUCCESS)
		delete w;

	return (bricks_handle_t)w;

}


void
bricks_kafka_destroy(bricks_handle_t kafka_h)
{
	((kafka_wrapper_t*)kafka_h)->destroy();
	delete (kafka_wrapper_t*)kafka_h;
}

bricks_error_code_e
bricks_kafka_register_topic(bricks_handle_t kafka_h, const char* topic)
{
	return ((kafka_wrapper_t*)kafka_h)->register_topic(topic);

}

BRICKSKAFKA_API bricks_error_code_e
bricks_kafka_unregister_topic(bricks_handle_t kafka_h, const char* topic)
{
	return ((kafka_wrapper_t*)kafka_h)->unregister_topic(topic);
}



#ifdef __cplusplus
}
#endif
