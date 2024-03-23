#pragma once
#include "bricks_api.h"
#include "services_api.h"

#ifdef BRICKSZEROMQ_EXPORTS
	#define BRICKSZEROMQ_API __declspec(dllexport)
#else
	#define BRICKSZEROMQ_API __declspec(dllimport)
#endif


BRICKSZEROMQ_API publisher_plugin_t*
create_zeromq_publisher();

BRICKSZEROMQ_API void
destroy_zeromq_publisher(publisher_plugin_t* service);

BRICKSZEROMQ_API subscriber_plugin_t*
create_zeromq_subscriber();

BRICKSZEROMQ_API void
destroy_zeromq_subscriber(subscriber_plugin_t * service);

BRICKSZEROMQ_API server_plugin_t* 
create_zeromq_router_server();

BRICKSZEROMQ_API void
destroy_zeromq_router_server(server_plugin_t*);

BRICKSZEROMQ_API bidi_plugin_t*
create_zeromq_dealer_bidi();

BRICKSZEROMQ_API void
destroy_zeromq_router_client(bidi_plugin_t*);