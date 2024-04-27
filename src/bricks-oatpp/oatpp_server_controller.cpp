#include "pch.h"
#include "oatpp_server_controller.h"
#include "oatpp_server.h"

using namespace std;
using namespace bricks;
using namespace bricks::plugins;

using namespace oatpp::web::server::api;
using namespace std::placeholders;


std::shared_ptr<oatpp_server_controller_t> 
oatpp_server_controller_t::create_shared(oatpp_server_t* server, std::shared_ptr<ObjectMapper> objectMapper) {
    return std::shared_ptr<oatpp_server_controller_t>(new oatpp_server_controller_t(server, objectMapper));
}

oatpp::async::CoroutineStarterForResult<const std::shared_ptr<oatpp::web::protocol::http::outgoing::Response>&>
oatpp_server_controller_t::proxy_method(const std::shared_ptr<oatpp::web::protocol::http::incoming::Request>& __request) 
{ 
    return brick_handler_t::startForResult(this, server, __request); 
}

oatpp_server_controller_t::oatpp_server_controller_t(oatpp_server_t* server, const std::shared_ptr<ObjectMapper>& objectMapper)
    : oatpp::web::server::api::ApiController(objectMapper), server(server)
{

};

void 
oatpp_server_controller_t::add_endpoint(const string& method, const string& path)
{
    auto handler = std::static_pointer_cast<Handler<oatpp_server_controller_t>>(getEndpointHandler(path));
    if (!handler)
    {
        handler = Handler<oatpp_server_controller_t>::createShared(this, nullptr, &oatpp_server_controller_t::proxy_method);
        setEndpointHandler(path, handler);
    }


    std::shared_ptr<oatpp::web::server::api::Endpoint::Info> info = getEndpointInfo(path);
    if (!info) 
    { 
        info = oatpp::web::server::api::Endpoint::Info::createShared(); 
        setEndpointInfo(path, info);
    }

    EndpointInfoBuilder info_builder = [=]() {
        auto _info = info;
        _info->name = path;
        _info->path = path;
        _info->method = method; 
        return _info; 
    };

    createEndpoint(
        m_endpoints,
        handler, 
        info_builder);
  
}

oatpp_server_controller_t::brick_handler_t::brick_handler_t(oatpp_server_controller_t* pController, oatpp_server_t* server, const std::shared_ptr<IncomingRequest>& pRequest)
    : HandlerCoroutine(pController, pRequest) , server(server), m_lockGuard(&lock)
{

}

void
oatpp_server_controller_t::brick_handler_t::responder(oatpp_server_controller_t::brick_handler_t* THIS, bricks_error_code_e err, const char* data, size_t size, xtree_t*)
{
    THIS->responded = true;

    THIS->err = err;

    if (data)
    {
        THIS->response = create_buffer(data, size);
    }

    THIS->cv.notifyFirst();

}


oatpp::async::Action 
oatpp_server_controller_t::brick_handler_t::act() {

    return  this->request->readBodyToStringAsync().callbackTo(&brick_handler_t::onBody);
    
}

oatpp::async::Action
oatpp_server_controller_t::brick_handler_t::onBody(const oatpp::String& body) {

    response_channel_t  f =
        std::bind(responder, this, _1, _2, _3, _4);

    auto xt = create_xtree();

    auto buf = create_buffer(body->data(), body->size());

    auto cb = std::bind(server->request_cb, f, buf, xt);

    server->cb_queue->enqueue(cb);

    return cv.wait(m_lockGuard, [this] {
                return this->responded; 
        }).next(yieldTo(&brick_handler_t::onReady));
}

oatpp::async::Action 
oatpp_server_controller_t::brick_handler_t::onReady() {
    OATPP_ASSERT(m_lockGuard.owns_lock()) // Now coroutine owns the lock

    return _return(controller->createResponse(err == BRICKS_SUCCESS ? 
        Status::CODE_200: Status::CODE_500 , response == nullptr ? "" : String(response->data(), response->size())));
}

oatpp_server_controller_t::brick_handler_t::~brick_handler_t()
{
    if (response)
        response->release();

    response = nullptr;
}
