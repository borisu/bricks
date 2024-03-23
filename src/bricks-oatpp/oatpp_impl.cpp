#include "pch.h"
#include "oatpp_api.h"
#include "oatpp_server.h"

BRICKSOATPP_API void
init_oatpp()
{
	oatpp::base::Environment::init();
}

BRICKSOATPP_API server_plugin_t*
create_oatpp_http_server()
{
	return new oatpp_server_t();
}

BRICKSOATPP_API  void
destroy_oatpp_http_server(server_plugin_t* h)
{
	delete h;
}

BRICKSOATPP_API void
destroy_oatpp()
{
	oatpp::base::Environment::destroy();
}