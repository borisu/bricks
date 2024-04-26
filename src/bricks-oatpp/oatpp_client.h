#pragma once
#include "plugin.h"

namespace bricks::plugins
{
	class oatpp_client_t :
		public client_plugin_t
	{
	public:

        oatpp_client_t();

		virtual bricks_error_code_e init(cb_queue_t* queue, const xtree_t* options) override;

		virtual bricks_error_code_e start() override;

		virtual bricks_error_code_e request(const char*, size_t, const xtree_t* options ) override;

		virtual void release() override { delete this; };

	private:

        std::shared_ptr < oatpp::parser::json::mapping::ObjectMapper> object_mapper;

		std::shared_ptr< oatpp::web::client::HttpRequestExecutor> executor;

        oatpp::async::Executor cor_executor;

		bool initiated;

		bool started;

		cb_queue_t* cb_queue = nullptr;

        string method;

        string path;
	};

	class api_client_t : public oatpp::web::client::ApiClient 
	{
	public: 

		api_client_t(const string &method, const string &path, const std::shared_ptr<oatpp::web::client::RequestExecutor>& requestExecutor, const std::shared_ptr<oatpp::data::mapping::ObjectMapper>& objectMapper);

		static std::shared_ptr<api_client_t> createShared(const string& method, const string& path, const std::shared_ptr<oatpp::web::client::RequestExecutor>& requestExecutor, const std::shared_ptr<oatpp::data::mapping::ObjectMapper>& objectMapper);
	
		static void add_headers(oatpp::web::client::ApiClient::Headers& headers, ...) { (void)headers; } 
		
		oatpp::async::CoroutineStarterForResult<const std::shared_ptr<oatpp::web::protocol::http::incoming::Response>&>
			getRequestStarter(const std::shared_ptr<oatpp::web::client::RequestExecutor::ConnectionHandle>& __connectionHandle = nullptr);

		string method;

		string path;

	};

    class client_coroutine_t : public oatpp::async::Coroutine<client_coroutine_t> 
    {
    private:
        std::shared_ptr<api_client_t> m_client;
        oatpp::String m_message;
    public:

        client_coroutine_t(const std::shared_ptr<api_client_t> client,
            const oatpp::String& message)
            : m_client(client)
            , m_message(message)
        {
        }

        static std::shared_ptr<client_coroutine_t> createShared(const std::shared_ptr<api_client_t> client,
            const oatpp::String& message)
        {
            return std::make_shared<client_coroutine_t>(client, message);
        }

        /**
         * Create DTO and send it via doPostWithDtoAsync method
         */
        Action act() override {
            return m_client->getRequestStarter().callbackTo(&client_coroutine_t::onResponse);
        }

        /**
         * Receive response and read body asynchronously
         */
        Action onResponse(const std::shared_ptr<oatpp::web::protocol::http::incoming::Response>& response) {
            return response->readBodyToStringAsync().callbackTo(&client_coroutine_t::onBody);
        }

        /**
         * Print read body from the stream and finish
         */
        Action onBody(const oatpp::String& body) {
            return finish();
        }

    };
	
}

