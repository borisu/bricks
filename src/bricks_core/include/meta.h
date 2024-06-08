#pragma once
#include "plugin.h"
#include "timer.h"

using namespace std;

namespace bricks 
{

	BRICKS_API client_plugin_t* create_pubsub_client(
		publisher_plugin_t* publisher,
		subscriber_plugin_t* subscriber,
		timer_t* timer,
		const char* request_topic_prefix,
		const char* reponse_topic_prefix,
		const char* error_topic_prefix);

	BRICKS_API server_plugin_t* create_pubsub_server(
		publisher_plugin_t* publisher,
		subscriber_plugin_t* subscriber,
		const char* request_topic_prefix,
		const char* reponse_topic_prefix,
		const char* error_topic_prefix);

}
