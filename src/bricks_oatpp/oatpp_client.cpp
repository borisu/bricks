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
		name    = get<string>(options->get_property_value("/bricks/oatpp/client", "name").value());
		host	= get<string>(options->get_property_value("/bricks/oatpp/client", "hostname").value());
		port	= get<int>(options->get_property_value("/bricks/oatpp/client", "port").value());
		method  = get<string>(options->get_property_value("/bricks/oatpp/client", "method").value());
		path    = get<string>(options->get_property_value("/bricks/oatpp/client", "path").value());

		auto hcount = get_opt<size_t>(options->get_node_children_count("/bricks/oatpp/client/headers"));

		headers = make_shared <oatpp::web::client::ApiClient::Headers>();

		for (int i = 0; i < hcount ; i++)
		{
			auto node = options->get_node_name(xp_t("/bricks/oatpp/client/headers", i));
			if (node.value() != "header")
				continue;
			
			auto header = get<string>(options->get_property_value(xp_t("/bricks/oatpp/client/headers", i), "name").value());
			auto value  = get<string>(options->get_property_value(xp_t("/bricks/oatpp/client/headers", i), "value").value());

			headers->put(oatpp::data::share::StringKeyLabelCI(header), oatpp::data::share::StringKeyLabel(value));
		}

		initiated = true;
	}
	catch (std::exception&)
	{
		return BRICKS_INVALID_PARAM;
	}

	return BRICKS_SUCCESS;
}
bricks_error_code_e 
oatpp_client_t::start()
{
	ASSERT_INITIATED;
	ASSERT_NOT_STARTED;

	started = true;

	return BRICKS_SUCCESS;
}

bricks_error_code_e 
oatpp_client_t::init_params(const xtree_t* options, const char* path, std::unordered_map<oatpp::String, oatpp::String>& params)
{
	if (!options)
		return BRICKS_SUCCESS;

	try {

		auto hcount = get_opt<size_t>(options->get_node_children_count(path));

		for (int i = 0; i < hcount; i++)
		{
			auto node = options->get_node_name(xp_t(path, i));
			if (node.value() != "parameter")
				continue;

			auto header = options->get_property_value_as_string(xp_t(path, i), "name").value();
			auto value = options->get_property_value_as_string(xp_t(path, i), "value").value();

			params[header] = value;
		}

		return BRICKS_SUCCESS;
	}
	catch (std::exception&)
	{
		return BRICKS_INVALID_PARAM;
	}

}

bricks_error_code_e 
oatpp_client_t::request(const char* data, size_t size, client_response_cb_t callback, const xtree_t* options)
{
	ASSERT_INITIATED;
	ASSERT_STARTED;

	bricks_error_code_e err = BRICKS_SUCCESS;

	std::unordered_map<oatpp::String, oatpp::String> path_params;
	std::unordered_map<oatpp::String, oatpp::String> query_params;

	if ((err = init_params(options,"/bricks/oatpp/request/path_parameters", path_params)) != BRICKS_SUCCESS)
	{
		return err;
	}

	if ((err = init_params(options, "/bricks/oatpp/request/query_parameters", query_params)) != BRICKS_SUCCESS)
	{
		return err;
	}

	auto api_client = std::make_shared<api_client_t>(
		method,
		path,
		path_params,
		query_params,
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
	std::unordered_map<oatpp::String, oatpp::String> &path_params,
	std::unordered_map<oatpp::String, oatpp::String> &query_params,
	const std::shared_ptr<oatpp::web::client::ApiClient::Headers>& headers,
	const std::shared_ptr<oatpp::web::protocol::http::outgoing::Body> body,
	const std::shared_ptr<oatpp::web::client::RequestExecutor>& requestExecutor,
	const std::shared_ptr<oatpp::data::mapping::ObjectMapper>& objectMapper):
	ApiClient(requestExecutor, objectMapper),
	path_params(path_params),
	query_params(query_params),
	method(method),
	path(path),
	headers(headers),
	body(body)
{

}

oatpp::async::CoroutineStarterForResult<const std::shared_ptr<oatpp::web::protocol::http::incoming::Response>&>
api_client_t::getRequestStarter(const std::shared_ptr<oatpp::web::client::RequestExecutor::ConnectionHandle>& connectionHandle)
{
	return executeRequestAsync(method, parsePathTemplate(path, path), *headers.get(), path_params, query_params, body, connectionHandle);
}

api_client_t::~api_client_t()
{
	
}


client_coroutine_t::client_coroutine_t(const std::shared_ptr<api_client_t> client, client_response_cb_t callback, cb_queue_t* cb_queue)
	: api_client(client),
	callback(callback),
	cb_queue(cb_queue)
{

}

/**
 * Create DTO and send it via doPostWithDtoAsync method
 */
oatpp::async::Action
client_coroutine_t::act()  {
	return api_client->getRequestStarter().callbackTo(&client_coroutine_t::onResponse);
}

/**
 * Receive response and read body asynchronously
 */
oatpp::async::Action
client_coroutine_t::onResponse(const std::shared_ptr<oatpp::web::protocol::http::incoming::Response>& response) {

	status_code = response->getStatusCode();

	response_xt = create_xtree();

	auto headers = response->getHeaders().getAll();
	for (auto& header : headers)
	{
		auto hnode = response_xt->add_node("/bricks/oatpp/response/headers/header", true);
		response_xt->set_property_value(hnode.value(), "name", header.first.toString());
		response_xt->set_property_value(hnode.value(), "value", header.second.toString());
	}

	return response->readBodyToStringAsync().callbackTo(&client_coroutine_t::onBody);
}

/**
 * Print read body from the stream and finish
 */

oatpp::async::Action 
client_coroutine_t::onBody(const oatpp::String& body) {

	auto buffer = create_buffer(body->data(), body->length());

	if (cb_queue)
	{
		cb_queue->enqueue(std::bind(callback, status_code == 200 ? BRICKS_SUCCESS : BRICKS_REMOTE_ERROR, buffer, response_xt));
	}
	else
	{
		try
		{
			callback(status_code == 200 ? BRICKS_SUCCESS : BRICKS_REMOTE_ERROR, buffer, response_xt);
		}
		catch (std::exception&) {}

	}

	return finish();
}