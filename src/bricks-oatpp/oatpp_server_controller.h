#pragma once
#include "oatpp/web/server/api/ApiController.hpp"
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/macro/component.hpp"

#include OATPP_CODEGEN_BEGIN(ApiController) //<-- Begin codegen


using namespace std;

namespace bricks::plugins
{

    class oatpp_server_controller_t : public oatpp::web::server::api::ApiController
    {
    public:

        oatpp_server_controller_t(const std::shared_ptr<ObjectMapper>& objectMapper);

        static std::shared_ptr<oatpp_server_controller_t> 
        create_shared(std::shared_ptr<ObjectMapper> objectMapper);

        void 
        add_endpoint(const string& method, const string& path);

        class brick_handler_t : public HandlerCoroutine<brick_handler_t, oatpp_server_controller_t> 
        {
        public:

            oatpp::async::Lock lock;

            oatpp::async::ConditionVariable cv;
            
            brick_handler_t(oatpp_server_controller_t* pController, const std::shared_ptr<IncomingRequest>& pRequest) : HandlerCoroutine(pController, pRequest) {}

              Action act() override {

                  return _return(controller->createResponse(Status::CODE_200, "dto"));
              }

        };

        oatpp::async::CoroutineStarterForResult<const std::shared_ptr<oatpp::web::protocol::http::outgoing::Response>&>
        proxy_method(const std::shared_ptr<oatpp::web::protocol::http::incoming::Request>& __request);
        
    };
}




