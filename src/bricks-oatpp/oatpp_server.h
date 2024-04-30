#pragma once
#include "plugin.h"

namespace bricks::plugins
{
	class oatpp_server_t :
		public server_plugin_t
	{
	public:

		oatpp_server_t();

		virtual bricks_error_code_e init(cb_queue_t* queue, const xtree_t* options) override;

		virtual bricks_error_code_e register_request_handler(server_request_cb_t request, const xtree_t* options ) override;

		virtual bricks_error_code_e start() override;

		virtual void release() override { delete this; };

		virtual ~oatpp_server_t();

	protected:

		static void oatpp_worker(oatpp_server_t* THIS);

	private:

		void destroy();

		cb_queue_t* cb_queue = nullptr;

		bool initiated = false;

		bool started = false;

		std::shared_ptr<oatpp::async::Executor> executor;

		std::shared_ptr<oatpp::network::ServerConnectionProvider> connectionProvider;

		std::shared_ptr<oatpp::web::server::HttpRouter>  router;

		std::shared_ptr<oatpp::network::ConnectionHandler> connectionHandler;

		std::shared_ptr<oatpp::data::mapping::ObjectMapper> objectMapper;

		std::shared_ptr<oatpp::network::Server> server;

		thread* server_thread = nullptr;

		atomic<bool> shutdown = false;

		server_request_cb_t request_cb;

		friend class oatpp_server_controller_t;


	};
	
}



