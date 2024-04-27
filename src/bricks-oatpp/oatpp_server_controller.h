#pragma once
#include "oatpp/web/server/api/ApiController.hpp"
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/macro/component.hpp"
#include "oatpp_server.h"

#include OATPP_CODEGEN_BEGIN(ApiController) //<-- Begin codegen


using namespace std;

namespace bricks::plugins
{

    class oatpp_server_controller_t : public oatpp::web::server::api::ApiController
    {
    public:

        oatpp_server_controller_t(oatpp_server_t* server, const std::shared_ptr<ObjectMapper>& objectMapper);

        static std::shared_ptr<oatpp_server_controller_t> 
        create_shared(oatpp_server_t* cb_queue, std::shared_ptr<ObjectMapper> objectMapper);

        void 
        add_endpoint(const string& method, const string& path);

        oatpp::async::CoroutineStarterForResult<const std::shared_ptr<oatpp::web::protocol::http::outgoing::Response>&>
        proxy_method(const std::shared_ptr<oatpp::web::protocol::http::incoming::Request>& __request);


        oatpp_server_t* server = nullptr;

        class brick_handler_t : public HandlerCoroutine<brick_handler_t, oatpp_server_controller_t>
        {
        public:

            bool responded = false;

            oatpp_server_t* server = nullptr;

            buffer_t *response = nullptr;

            bricks_error_code_e err = BRICKS_SUCCESS;

            oatpp::async::Lock lock;

            oatpp::async::ConditionVariable cv;

            oatpp::async::LockGuard m_lockGuard;

            brick_handler_t(oatpp_server_controller_t* pController, oatpp_server_t* server, const std::shared_ptr<IncomingRequest>& pRequest);

            oatpp::async::Action act() override;

            oatpp::async::Action onReady();

            oatpp::async::Action onBody(const oatpp::String& body);

            static void responder(oatpp_server_controller_t::brick_handler_t* THIS, bricks_error_code_e err, const char* data, size_t size,xtree_t*);

            virtual ~brick_handler_t();

        };
        
    };
}




