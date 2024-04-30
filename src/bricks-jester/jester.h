#pragma once
#include "bricks.h"

#ifdef BRICKSJESTER_EXPORTS
#define BRICKSJESTER_API __declspec(dllexport)
#else
#define BRICKSJESTER_API __declspec(dllimport)
#endif

namespace bricks::plugins {

	BRICKSJESTER_API brick_t*
		create_jester_pubsub_context();

	BRICKSJESTER_API publisher_plugin_t*
		create_jester_publisher(brick_t* ctx);

	BRICKSJESTER_API subscriber_plugin_t*
		create_jester_subscriber(brick_t* ctx);

	BRICKSJESTER_API brick_t*
		create_jester_bidi_context();

	BRICKSJESTER_API bidi_plugin_t*
		create_jester_bidi(brick_t* ctx, int);

	/*BRICKSJESTER_API server_plugin_t*
		create_jester_server(brick_t* ctx);

	BRICKSJESTER_API client_plugin_t*
		create_jester_client(brick_t* ctx);*/

}




