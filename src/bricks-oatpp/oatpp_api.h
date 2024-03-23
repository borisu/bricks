#pragma once
#include "services_api.h"

#ifdef BRICKSOATPP_EXPORTS
#define BRICKSOATPP_API __declspec(dllexport)
#else
#define BRICKSOATPP_API __declspec(dllimport)
#endif


BRICKSOATPP_API void 
init_oatpp();

BRICKSOATPP_API server_plugin_t*
create_oatpp_http_server();

BRICKSOATPP_API void
destroy_oatpp_http_server(server_plugin_t* h);

BRICKSOATPP_API void
destroy_oatpp();
