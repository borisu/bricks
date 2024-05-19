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
oatpp_server_controller_t::brick_handler_t::server_proxy(bricks_error_code_e err, const char* data, size_t size, xtree_t* rsp_options)
{
    this->responded   = true;
    this->err         = err;
    this->rsp_options = rsp_options;

    if (data)
    {
        response = create_buffer(data, size);
    }

    cv.notifyFirst();

}


oatpp::async::Action 
oatpp_server_controller_t::brick_handler_t::act() {

    return  this->request->readBodyToStringAsync().callbackTo(&brick_handler_t::onBody);
    
}

oatpp::async::Action
oatpp_server_controller_t::brick_handler_t::onBody(const oatpp::String& body) {

   
    auto xt = create_xtree();

    auto headers = this->request->getHeaders().getAll();
    for (auto &header : headers)
    {
        auto hnode = xt->add_node("/bricks/oatpp/request/headers/header", true);
        xt->set_property_value(hnode.value(), "name", header.first.toString());
        xt->set_property_value(hnode.value(), "value", header.second.toString());
    }

    auto path_vars = this->request->getPathVariables().getVariables();
    for (auto& path_var : path_vars)
    {
        auto hnode = xt->add_node("/bricks/oatpp/request/path_parameters/parameter", true);
        xt->set_property_value(hnode.value(), "name", path_var.first.toString());
        xt->set_property_value(hnode.value(), "value", path_var.second.toString());
    }

    auto query_vars = this->request->getQueryParameters().getAll();
    for (auto& query_var : query_vars)
    {
        auto hnode = xt->add_node("/bricks/oatpp/request/query_parameters/parameter", true);
        xt->set_property_value(hnode.value(), "name", query_var.first.toString());
        xt->set_property_value(hnode.value(), "value", query_var.second.toString());
    }

    response_proxy_cb_t  f =
        std::bind(&oatpp_server_controller_t::brick_handler_t::server_proxy, this, _1, _2, _3, _4);

    server->cb_queue->enqueue(
        std::bind(server->request_cb, f, create_buffer(body->data(), body->size()), xt));
   
    

    return cv.wait(m_lockGuard, [this] {
                return this->responded; 
        }).next(yieldTo(&brick_handler_t::onReady));
}

oatpp::async::Action 
oatpp_server_controller_t::brick_handler_t::onReady() {
    OATPP_ASSERT(m_lockGuard.owns_lock()) // Now coroutine owns the lock

    auto http_response = controller->createResponse(err == BRICKS_SUCCESS ?
        Status::CODE_200 : Status::CODE_500, response == nullptr ? "" : String(response->data(), response->size()));

    if (rsp_options)
    {
        auto hcount = get_opt<size_t>(rsp_options->get_node_children_count("/bricks/oatpp/response/headers"));

        for (int i = 0; i < hcount; i++)
        {
            auto node = rsp_options->get_node_name(xp_t("/bricks/oatpp/response/headers", i));
            if (node.value() != "header")
                continue;

            auto header = rsp_options->get_property_value_as_string(xp_t("/bricks/oatpp/response/headers", i), "name").value();
            auto value = rsp_options->get_property_value_as_string(xp_t("/bricks/oatpp/response/headers", i), "value").value();
            http_response->putHeader(header, value);

        }

    }

    return _return(http_response);
}

oatpp_server_controller_t::brick_handler_t::~brick_handler_t()
{
    if (response)
    {
        response->release();
        response = nullptr;
    }

    if (rsp_options)
    {
        rsp_options->release();
        rsp_options = nullptr;
    }

    response = nullptr;
}
