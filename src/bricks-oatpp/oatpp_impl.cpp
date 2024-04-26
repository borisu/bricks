#include "pch.h"
#include "oatpp_plugin.h"
#include "oatpp_server.h"
#include "oatpp_client.h"

using namespace bricks;
using namespace bricks::plugins;

BRICKSOATPP_API void
bricks::plugins::init_oatpp()
{
	oatpp::base::Environment::init();
}

BRICKSOATPP_API server_plugin_t*
bricks::plugins::create_oatpp_http_server()
{
	return new oatpp_server_t();
}

BRICKSOATPP_API client_plugin_t*
bricks::plugins::create_oatpp_http_client()
{
	return new oatpp_client_t();
}


BRICKSOATPP_API void
bricks::plugins::destroy_oatpp()
{
	oatpp::base::Environment::destroy();
}