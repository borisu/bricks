#include "pch.h"
#include "zeromq_api.h"
#include "zeromq_publisher.h"
#include "zeromq_subscriber.h"
#include "zeromq_router_client.h"
#include "zeromq_router_server.h"

BRICKSZEROMQ_API publisher_service_t*
create_zeromq_publisher()
{
	return new zeromq_publisher_t();
}

BRICKSZEROMQ_API void
destroy_zeromq_publisher(publisher_service_t* service)
{
	delete service;
}

BRICKSZEROMQ_API subscriber_service_t*
create_zeromq_subscriber()
{
	return new zeromq_subscriber_t();
}

BRICKSZEROMQ_API void
destroy_zeromq_subscriber(subscriber_service_t* service)
{
	delete service;
}

BRICKSZEROMQ_API server_service_t*
create_zeromq_router_server()
{
	return new zeromq_router_server_t();
}

BRICKSZEROMQ_API void
destroy_zeromq_router_server(server_service_t* service)
{
	delete service;
}


BRICKSZEROMQ_API client_service_t*
create_zeromq_router_client()
{
	return new zeromq_router_client_t();
}

BRICKSZEROMQ_API void
destroy_zeromq_router_client(client_service_t* service)
{
	delete service;
}