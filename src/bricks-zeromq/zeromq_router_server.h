#pragma once
#include "services_api.h"
#include "zeromq_service.h"

using namespace std;

class zeromq_router_server_t :
    public server_service_t,
    public zeromq_service_t
{
public:

    zeromq_router_server_t();

    virtual bricks_error_code_e init(const xtree_t* options) override;

    virtual bricks_error_code_e register_request_handler(void* opaque, request_cb_t request, const xtree_t* options) override;

    virtual bricks_error_code_e send_response(bricks_handle_t, const char*, size_t, const xtree_t* options) override;

    virtual bricks_error_code_e poll(size_t timeout);

    virtual ~zeromq_router_server_t();

private:

    void destroy();

    void* router = nullptr;

    void* context = nullptr;
    
    void* opaque = nullptr;

    zmq_pollitem_t items[1] = { 0 };

    enum RCV_STATE
    {
        NEXT_IS_IDENTITY,
        NEXT_IS_MESSAGE
    } rcv_state = NEXT_IS_IDENTITY;

    zmq_msg_t *identity = nullptr;

    zmq_msg_t *request = nullptr;

    request_cb_t req_cb;


};

