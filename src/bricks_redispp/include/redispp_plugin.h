#pragma once
#include "bricks.h"

#ifdef WIN32
#ifdef BRICKSREDISPP_EXPORTS
#define BRICKREDISPP_API __declspec(dllexport)
#else
#define BRICKREDISPP_API __declspec(dllimport)
#endif
#else
#define BRICKREDISPP_API
#endif 


namespace bricks::plugins {

	BRICKREDISPP_API publisher_plugin_t*
		create_redispp_publisher();

	BRICKREDISPP_API subscriber_plugin_t*
		create_redispp_subscriber();

}
