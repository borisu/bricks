#include "pch.h"
#include "utils.h"
#include "oatpp_client.h"

using namespace std;
using namespace bricks;
using namespace bricks::plugins;

oatpp_client_t::oatpp_client_t()
	:cor_executor(1,1,1)
{

}

bricks_error_code_e 
oatpp_client_t::init(cb_queue_t* queue, const xtree_t* options)
{
	ASSERT_NOT_INITIATED;
	ASSERT_NOT_STARTED;

	cb_queue = queue;

	try
	{
		string host		= get<string>(options->get_property_value("/bricks/oatpp_http_client/client", "hostname").value());
		uint16_t port	= get<int>(options->get_property_value("/bricks/oatpp_http_client/client", "port").value());
		method          = get<string>(options->get_property_value("/bricks/oatpp_http_client/client", "method").value());
		path            = get<string>(options->get_property_value("/bricks/oatpp_http_client/client", "path").value());

		object_mapper = oatpp::parser::json::mapping::ObjectMapper::createShared();

		auto connection_provider = oatpp::network::tcp::client::ConnectionProvider::createShared({ host, port });

		executor = oatpp::web::client::HttpRequestExecutor::createShared(connection_provider);
	
		initiated = true;
	}
	catch (std::exception& )
	{
		return BRICKS_INVALID_PARAM;
	}

	initiated = true;

	return BRICKS_SUCCESS;
}
bricks_error_code_e 
oatpp_client_t::start()
{
	ASSERT_INITIATED;

	started = true;

	return BRICKS_SUCCESS;

}


bricks_error_code_e 
oatpp_client_t::request(const char* data, size_t size, const xtree_t* options)
{

	ASSERT_INITIATED;
	ASSERT_STARTED;

	auto client = api_client_t::createShared(method, path, executor, object_mapper);

	const oatpp::String message(data, size);

	auto cor = client_coroutine_t::createShared(client, message);

	cor_executor.execute<client_coroutine_t>(client, message);

	return BRICKS_SUCCESS;

}


api_client_t::api_client_t(const string& method, const string& path, const std::shared_ptr<oatpp::web::client::RequestExecutor>& requestExecutor, const std::shared_ptr<oatpp::data::mapping::ObjectMapper>& objectMapper)
	: oatpp::web::client::ApiClient(requestExecutor, objectMapper),
	method(method),
	path(path)
{

}

std::shared_ptr<api_client_t> 
api_client_t::createShared(const string& method, const string& path, const std::shared_ptr<oatpp::web::client::RequestExecutor>& requestExecutor, const std::shared_ptr<oatpp::data::mapping::ObjectMapper>& objectMapper)
{
	return std::make_shared<api_client_t>(method, path, requestExecutor, objectMapper);
}

oatpp::async::CoroutineStarterForResult<const std::shared_ptr<oatpp::web::protocol::http::incoming::Response>&>
api_client_t::getRequestStarter(const std::shared_ptr<oatpp::web::client::RequestExecutor::ConnectionHandle>& __connectionHandle)
{
	const oatpp::data::share::StringTemplate Z_PATH_TEMPLATE_getRootAsync = parsePathTemplate(path, path);

	oatpp::web::client::ApiClient::Headers __headers; 
	
	add_headers(__headers, 1);

	std::shared_ptr<oatpp::web::protocol::http::outgoing::Body> body;
	

	return executeRequestAsync(method, Z_PATH_TEMPLATE_getRootAsync, __headers, {}, {}, body, __connectionHandle);

}