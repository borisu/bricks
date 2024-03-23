#pragma once
#include "services_api.h"
#include "zeromq_service.h"

class zeromq_dealer_bidi_t: 
	public bidi_plugin_t,
	public zeromq_service_t
{
public:

    zeromq_dealer_bidi_t();

	virtual bricks_error_code_e init(const xtree_t* options) override;

	virtual bricks_error_code_e register_event_handler(void* opaque, event_cb_t request, const xtree_t* options) override;

	virtual bricks_error_code_e send_event(const char*, size_t, const xtree_t* options) override;

    virtual bricks_error_code_e poll(size_t timeout);

    virtual ~zeromq_dealer_bidi_t();

private:

    void destroy();

    void* router = nullptr;

    void* context = nullptr;

    void* opaque = nullptr;

    zmq_pollitem_t items[1] = { 0 };

    event_cb_t rsp_cb;

};

