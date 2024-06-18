#pragma once
#include "bricks.h"

#ifdef WIN32
#ifdef BRICKSLIBEVENT_EXPORTS
#define BRICKSLIBEVENT_API __declspec(dllexport)
#else
#define BRICKSLIBEVENT_API __declspec(dllimport)
#endif
#else
#define BRICKSKAFKA_API
#endif 


namespace bricks::plugins {

	BRICKSLIBEVENT_API bricks_error_code_e
		libevent_init();

	BRICKSLIBEVENT_API client_plugin_t*
		create_libevent_client();

	BRICKSLIBEVENT_API server_plugin_t*
		create_libevent_server();

	BRICKSLIBEVENT_API client_plugin_t*
		create_libevent_client();

	BRICKSLIBEVENT_API void
		libevent_destroy();

}