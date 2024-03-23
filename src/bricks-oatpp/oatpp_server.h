#pragma once
#include "services_api.h"
#include "bricks_api.h"

class oatpp_server_t:
	public server_plugin_t
{
public:

	oatpp_server_t();

	virtual bricks_error_code_e init(const xtree_t* options) override;

	virtual bricks_error_code_e register_request_handler(void* opaque, request_cb_t request, const xtree_t* options) override;

	virtual bricks_error_code_e send_response(bricks_handle_t ctx , const char*, size_t, const xtree_t* options) override;

	virtual bricks_error_code_e poll(size_t timeout);

	virtual ~oatpp_server_t();

private:

	void destroy();

	request_cb_t req_cb;

};

