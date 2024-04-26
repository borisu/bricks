#include "pch.h"
#include "oatpp_server.h"
#include "utils.h"
#include "oatpp_server_controller.h"

using namespace bricks;
using namespace bricks::plugins;

oatpp_server_t::oatpp_server_t()
{

};

void 
oatpp_server_t::destroy()
{
	initiated = false;
	started = false;

	shutdown = true;

	if (server_thread)
		server_thread->join();

	if (server)
		server->stop();

	if (executor)
	{
		executor->stop();
		executor->join();
	}
}

bricks_error_code_e 
oatpp_server_t::init(cb_queue_t* queue, const xtree_t* options)
{
	ASSERT_NOT_INITIATED;
	ASSERT_NOT_STARTED;

	try
	{
		string host		= get<string>(options->get_property_value("/bricks/oatpp_http_server/server", "hostname").value());
		uint16_t port	= get<int>(options->get_property_value("/bricks/oatpp_http_server/server", "port").value());
		int version		= get<int>(options->get_property_value("/bricks/oatpp_http_server/server", "ipversion").value());

		// https://github.com/oatpp/example-async-api/blob/master/src/AppComponent.hpp

		executor = std::make_shared<oatpp::async::Executor>(
			9 /* Data-Processing threads */,
			2 /* I/O threads */,
			1 /* Timer threads */
		);

		connectionProvider = oatpp::network::tcp::server::ConnectionProvider::createShared({ host, port , version == 6 ? oatpp::network::Address::IP_6 : oatpp::network::Address::IP_4 });

		// Create a router
		router = oatpp::web::server::HttpRouter::createShared();

		connectionHandler = oatpp::web::server::AsyncHttpConnectionHandler::createShared(router, executor);

		auto serializerConfig = oatpp::parser::json::mapping::Serializer::Config::createShared();
		auto deserializerConfig = oatpp::parser::json::mapping::Deserializer::Config::createShared();
		deserializerConfig->allowUnknownFields = true;

		objectMapper = oatpp::parser::json::mapping::ObjectMapper::createShared(serializerConfig, deserializerConfig);

		auto c = oatpp_server_controller_t::create_shared(objectMapper);

		size_t ep_size = options->get_node_children_count("/bricks/oatpp_http_server/endpoints").value();
		if (ep_size == 0)
			return BRICKS_INVALID_PARAM;

		for (int i = 0; i < ep_size; i++)
		{
			auto nname = options->get_node_name(xp_t("/bricks/oatpp_http_server/endpoints", i));
			if (nname.value() != "endpoint")
				continue;

			string method = get<string>(options->get_property_value(xp_t("/bricks/oatpp_http_server/endpoints", i), "method").value());
			string path   = get<string>(options->get_property_value(xp_t("/bricks/oatpp_http_server/endpoints", i), "path").value());

			c->add_endpoint(method, path);
			
		}
		
		router->addController(c);

		server = oatpp::network::Server::createShared(connectionProvider, connectionHandler);

		initiated = true;

	}
	catch (std::exception&)
	{
		return BRICKS_FAILURE_GENERIC;
	}

	return BRICKS_SUCCESS;
}

bricks_error_code_e 
oatpp_server_t::register_request_handler(request_cb_t request, const xtree_t* options)
{
	request_cb = request;

	return BRICKS_SUCCESS;
};

bricks_error_code_e 
oatpp_server_t::send_response(bricks_handle_t ctx, const char*, size_t, const xtree_t* options)
{
	return BRICKS_SUCCESS;
}


void 
oatpp_server_t::oatpp_worker(oatpp_server_t* THIS)
{

	// Start the server
	THIS->server->run([THIS]() { return THIS->shutdown == false; });

}

bricks_error_code_e 
oatpp_server_t::start()
{
	ASSERT_NOT_STARTED;

	server_thread = new thread(&oatpp_server_t::oatpp_worker, this);

	started = true;

	return BRICKS_SUCCESS;
}

oatpp_server_t::~oatpp_server_t()
{
	destroy();
}
