#pragma once
#include "bricks.h"

#ifdef BRICKSOATPP_EXPORTS
#define BRICKSOATPP_API __declspec(dllexport)
#else
#define BRICKSOATPP_API __declspec(dllimport)
#endif

using namespace bricks;

namespace bricks::plugins
{
	BRICKSOATPP_API void
		init_oatpp();

	BRICKSOATPP_API server_plugin_t*
		create_oatpp_http_server();

	BRICKSOATPP_API client_plugin_t*
		create_oatpp_http_client();

	BRICKSOATPP_API void
		destroy_oatpp();

}


