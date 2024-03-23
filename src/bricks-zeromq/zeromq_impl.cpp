#include "pch.h"
#include "zeromq_api.h"
#include "zeromq_publisher.h"
#include "zeromq_subscriber.h"
#include "zeromq_dealer_bidi.h"
#include "zeromq_router_server.h"

BRICKSZEROMQ_API publisher_plugin_t*
create_zeromq_publisher()
{
	return new zeromq_publisher_t();
}

BRICKSZEROMQ_API void
destroy_zeromq_publisher(publisher_plugin_t* service)
{
	delete service;
}

BRICKSZEROMQ_API subscriber_plugin_t*
create_zeromq_subscriber()
{
	return new zeromq_subscriber_t();
}

BRICKSZEROMQ_API void
destroy_zeromq_subscriber(subscriber_plugin_t* service)
{
	delete service;
}

BRICKSZEROMQ_API server_plugin_t*
create_zeromq_router_server()
{
	return new zeromq_router_server_t();
}

BRICKSZEROMQ_API void
destroy_zeromq_router_server(server_plugin_t* service)
{
	delete service;
}

BRICKSZEROMQ_API bidi_plugin_t*
create_zeromq_dealer_bidi()
{
	return new zeromq_dealer_bidi_t();
}

BRICKSZEROMQ_API void
destroy_zeromq_router_client(bidi_plugin_t* service)
{
	delete service;
}