#pragma once
#include "bricks.h"

#ifdef WIN32
#ifdef BRICKSRABBITMQ_EXPORTS
#define BRICKSRABBITMQ_API __declspec(dllexport)
#else
#define BRICKSRABBITMQ_API __declspec(dllimport)
#endif
#else
#define BRICKSRABBITMQ_API
#endif 


namespace bricks::plugins {

	BRICKSRABBITMQ_API publisher_plugin_t*
		create_rabbitmq_publisher();

	BRICKSRABBITMQ_API subscriber_plugin_t*
		create_rabbitmq_subscriber();

	

}
