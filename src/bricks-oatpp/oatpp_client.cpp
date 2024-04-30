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

oatpp_client_t::~oatpp_client_t()
{
	destroy();
}

void
oatpp_client_t::destroy()
{
	cor_executor.stop();
	cor_executor.join();
}

bricks_error_code_e 
oatpp_client_t::init(cb_queue_t* queue, const xtree_t* options)
{
	ASSERT_NOT_INITIATED;
	ASSERT_NOT_STARTED;

	cb_queue = queue;

	try
	{
		host	= get<string>(options->get_property_value("/bricks/oatpp_http_client/client", "hostname").value());
		port	= get<int>(options->get_property_value("/bricks/oatpp_http_client/client", "port").value());
		method  = get<string>(options->get_property_value("/bricks/oatpp_http_client/client", "method").value());
		path    = get<string>(options->get_property_value("/bricks/oatpp_http_client/client", "path").value());

		
		auto hcount = get_opt<size_t>(options->get_node_children_count("/bricks/oatpp_http_client/client/headers"));

		headers = make_shared <oatpp::web::client::ApiClient::Headers>();

		for (int i = 0; i < 0; i++)
		{
			auto node = options->get_node_name(xp_t("/bricks/oatpp_http_client/client/headers", i));
			if (node.value() != "header")
				continue;
			
			auto header = get<string>(options->get_property_value(xp_t("/bricks/oatpp_http_client/client/headers", i),  "name").value());
			auto value  = get<string>( options->get_property_value(xp_t("/bricks/oatpp_http_client/client/headers", i), "value").value());

			headers->put(oatpp::data::share::StringKeyLabelCI(header), oatpp::data::share::StringKeyLabel(value));
		}

	
		initiated = true;
	}
	catch (std::exception& )
	{
		return BRICKS_INVALID_PARAM;
	}

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
oatpp_client_t::request(const char* data, size_t size, client_response_cb_t callback, const xtree_t* options)
{
	ASSERT_INITIATED;
	ASSERT_STARTED;

	auto api_client = api_client_t::createShared(
		method,
		path,
		headers,
		oatpp::web::protocol::http::outgoing::BufferBody::createShared(oatpp::String(data, size)), 
		oatpp::web::client::HttpRequestExecutor::createShared(oatpp::network::tcp::client::ConnectionProvider::createShared({ host, port })),
		oatpp::parser::json::mapping::ObjectMapper::createShared());

	cor_executor.execute<client_coroutine_t>(api_client, callback, cb_queue);

	return BRICKS_SUCCESS;

}


api_client_t::api_client_t(
	const oatpp::String& method,
	const oatpp::String& path,
	const std::shared_ptr<oatpp::web::client::ApiClient::Headers>& headers,
	const std::shared_ptr<oatpp::web::protocol::http::outgoing::Body> body,
	const std::shared_ptr<oatpp::web::client::RequestExecutor>& requestExecutor,
	const std::shared_ptr<oatpp::data::mapping::ObjectMapper>& objectMapper):
	ApiClient(requestExecutor, objectMapper),
	method(method),
	path(path),
	headers(headers),
	body(body)
{

}

std::shared_ptr<api_client_t> 
api_client_t::createShared(
	const oatpp::String& method,
	const oatpp::String& path,
	const std::shared_ptr<oatpp::web::client::ApiClient::Headers>& headers,
	const std::shared_ptr<oatpp::web::protocol::http::outgoing::Body>& body,
	const std::shared_ptr<oatpp::web::client::RequestExecutor>& requestExecutor,
	const std::shared_ptr<oatpp::data::mapping::ObjectMapper>& objectMapper)
{
	return std::make_shared<api_client_t>(method,path, headers,body, requestExecutor, objectMapper);
}

oatpp::async::CoroutineStarterForResult<const std::shared_ptr<oatpp::web::protocol::http::incoming::Response>&>
api_client_t::getRequestStarter(const std::shared_ptr<oatpp::web::client::RequestExecutor::ConnectionHandle>& connectionHandle)
{

	return executeRequestAsync(method, parsePathTemplate(path, path), *headers.get(), {}, {}, body, connectionHandle);

}

api_client_t::~api_client_t()
{
	
}