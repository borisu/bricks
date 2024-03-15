#include "pch.h"
#include "zeromq_api.h"
#include "zeromq_publisher.h"
#include "zeromq_subscriber.h"

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
