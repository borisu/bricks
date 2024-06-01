#pragma once
#include "plugin.h"
#include "utils.h"

namespace bricks::plugins
{
    class api_client_t;

    class client_coroutine_t;

	class oatpp_client_t :
		public client_plugin_t
	{
	public:

        oatpp_client_t();

		virtual bricks_error_code_e init(cb_queue_t* queue, int timeout_ms, const xtree_t* options) override;

		virtual bricks_error_code_e start() override;

		virtual bricks_error_code_e issue_request(const char*, size_t, response_cb_t, const xtree_t* options) override;

		virtual void release() override { delete this; };

        virtual ~oatpp_client_t();

        virtual bool check_capability(bricks::plugin_capabilities_e) override;

	private:

        void destroy();

        virtual bricks_error_code_e init_params(const xtree_t* options, const char* path, std::unordered_map<oatpp::String, oatpp::String> &params);

        oatpp::async::Executor cor_executor;

        std::shared_ptr<oatpp::web::client::ApiClient::Headers> headers;

		bool initiated = false;

		bool started = false;

		cb_queue_t* cb_queue = nullptr;

        string method;

        string path;

        string host;

        string name;

        uint16_t port = 80;

        int timeout_ms = BRICKS_DEFAULT_CLIENT_TIMEOUT;

        friend api_client_t;

        friend client_coroutine_t;

        std::recursive_mutex mtx;

	};


	class api_client_t : public oatpp::web::client::ApiClient 
	{
	public: 

		api_client_t(
            const oatpp::String& method,
            const oatpp::String& path,
            std::unordered_map<oatpp::String, oatpp::String> &path_params,
            std::unordered_map<oatpp::String, oatpp::String> &query_params,
            const std::shared_ptr<oatpp::web::client::ApiClient::Headers>  &headers,
            const std::shared_ptr<oatpp::web::protocol::http::outgoing::Body> body,
            const std::shared_ptr<oatpp::web::client::RequestExecutor>& requestExecutor, 
            const std::shared_ptr<oatpp::data::mapping::ObjectMapper>& objectMapper);

		oatpp::async::CoroutineStarterForResult<const std::shared_ptr<oatpp::web::protocol::http::incoming::Response>&>
			getRequestStarter(
                const std::shared_ptr<oatpp::web::client::RequestExecutor::ConnectionHandle>& connectionHandle = nullptr);

        virtual ~api_client_t();

        friend client_coroutine_t;

    private:

        const oatpp::String method;

        const oatpp::String path;

        const std::shared_ptr<oatpp::web::client::ApiClient::Headers>  headers;

        const std::shared_ptr<oatpp::web::protocol::http::outgoing::Body> body;

        std::unordered_map<oatpp::String, oatpp::String> path_params;

        std::unordered_map<oatpp::String, oatpp::String> query_params;

	};

    class client_coroutine_t : public oatpp::async::Coroutine<client_coroutine_t> 
    {
    private:

        std::shared_ptr<api_client_t> api_client;

        v_int32 status_code = 0;

        cb_queue_t* cb_queue = nullptr;

        response_cb_t callback = nullptr;

        xtree_t* response_xt = nullptr;

    public:

        client_coroutine_t(const std::shared_ptr<api_client_t> client, response_cb_t callback, cb_queue_t* cb_queue);

        /**
         * Create DTO and send it via doPostWithDtoAsync method
         */
        virtual Action act() override;

        /**
         * Receive response and read body asynchronously
         */
        Action onResponse(const std::shared_ptr<oatpp::web::protocol::http::incoming::Response>& response);

        /**
         * Print read body from the stream and finish
         */
        Action onBody(const oatpp::String& body);

        virtual ~client_coroutine_t();

    };
	
}

