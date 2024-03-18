#pragma once
#include "services_api.h"
#include "zeromq_service.h"

class zeromq_router_client_t: 
	public client_service_t,
	public zeromq_service_t
{
public:

    zeromq_router_client_t();

	virtual bricks_error_code_e init(const xtree_t* options) override;

	virtual bricks_error_code_e register_client(void* opaque, response_cb_t request, const xtree_t* options) override;

	virtual bricks_error_code_e issue_request(const char*, size_t, const xtree_t* options) override;

    virtual bricks_error_code_e poll(size_t timeout);

    virtual ~zeromq_router_client_t();

private:

    void destroy();

    void* router = nullptr;

    void* context = nullptr;

    void* opaque = nullptr;

    zmq_pollitem_t items[1] = { 0 };

    response_cb_t rsp_cb;

};

