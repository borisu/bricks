#pragma once
#include "bricks.h"

#ifdef WIN32
#ifdef BRICKSREDISPP_EXPORTS
#define BRICKSREDISPP_API __declspec(dllexport)
#else
#define BRICKSREDISPP_API __declspec(dllimport)
#endif
#else
#define BRICKSREDISPP_API
#endif 


namespace bricks::plugins {

	BRICKSREDISPP_API publisher_plugin_t*
		create_redispp_publisher();

	BRICKSREDISPP_API subscriber_plugin_t*
		create_redispp_subscriber();

	BRICKSREDISPP_API subscriber_plugin_t*
		create_redispp_psubscriber();

}
