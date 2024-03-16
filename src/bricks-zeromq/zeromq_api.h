#pragma once
#include "bricks_api.h"
#include "services_api.h"

#ifdef BRICKSZEROMQ_EXPORTS
	#define BRICKSZEROMQ_API __declspec(dllexport)
#else
	#define BRICKSZEROMQ_API __declspec(dllimport)
#endif


BRICKSZEROMQ_API publisher_service_t*
create_zeromq_publisher();

BRICKSZEROMQ_API void
destroy_zeromq_publisher(publisher_service_t* service);

BRICKSZEROMQ_API subscriber_service_t*
create_zeromq_subscriber();

BRICKSZEROMQ_API void
destroy_zeromq_subscriber(subscriber_service_t * service);

BRICKSZEROMQ_API server_service_t* 
create_zeromq_router_server();

BRICKSZEROMQ_API void
destroy_zeromq_router_server(server_service_t*);

BRICKSZEROMQ_API client_service_t*
create_zeromq_router_client();

BRICKSZEROMQ_API void
destroy_zeromq_router_client(client_service_t*);