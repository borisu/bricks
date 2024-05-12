#include "pch.h"
#include "zeromq_plugin.h"
#include "zeromq_publisher.h"
#include "zeromq_subscriber.h"
#include "zeromq_bidi.h"

using namespace bricks;
using namespace bricks::plugins;

BRICKSZEROMQ_API publisher_plugin_t*
bricks::plugins::create_zeromq_publisher()
{
	return new zeromq_publisher_t();
}

BRICKSZEROMQ_API subscriber_plugin_t*
bricks::plugins::create_zeromq_subscriber()
{
	return new zeromq_subscriber_t();
}


BRICKSZEROMQ_API bidi_plugin_t*
bricks::plugins::create_zeromq_bidi()
{
	return new zeromq_bidi_t();
}

