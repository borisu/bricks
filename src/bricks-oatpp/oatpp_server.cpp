#include "pch.h"
#include "oatpp_server.h"


oatpp_server_t::oatpp_server_t()
{

};

bricks_error_code_e 
oatpp_server_t::init(const xtree_t* options)
{
	return BRICKS_SUCCESS;
}

bricks_error_code_e 
oatpp_server_t::register_request_handler(void* opaque, request_cb_t request, const xtree_t* options)
{
	return BRICKS_SUCCESS;
};

bricks_error_code_e 
oatpp_server_t::send_response(bricks_handle_t ctx, const char*, size_t, const xtree_t* options)
{
	return BRICKS_SUCCESS;
}


bricks_error_code_e 
oatpp_server_t::poll(size_t timeout)
{
	return BRICKS_SUCCESS;
}

oatpp_server_t::~oatpp_server_t()
{

}
