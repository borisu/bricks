#include "pch.h"
#include "oatpp_server_controller.h"

using namespace std;
using namespace bricks;
using namespace bricks::plugins;

using namespace oatpp::web::server::api;


std::shared_ptr<oatpp_server_controller_t> 
oatpp_server_controller_t::create_shared(std::shared_ptr<ObjectMapper> objectMapper) {
    return std::shared_ptr<oatpp_server_controller_t>(new oatpp_server_controller_t(objectMapper));
}

oatpp::async::CoroutineStarterForResult<const std::shared_ptr<oatpp::web::protocol::http::outgoing::Response>&>
oatpp_server_controller_t::proxy_method(const std::shared_ptr<oatpp::web::protocol::http::incoming::Request>& __request) 
{ 
    return brick_handler_t::startForResult(this, __request); 
}

oatpp_server_controller_t::oatpp_server_controller_t(const std::shared_ptr<ObjectMapper>& objectMapper)
    : oatpp::web::server::api::ApiController(objectMapper) 
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
