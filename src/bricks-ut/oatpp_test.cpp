#include "pch.h"

#ifndef NO_OATPP_PLUGIN

#include "generic_tests.h"
#include "oatpp_plugin.h"

using namespace std;
using namespace bricks;
using namespace bricks::plugins;

TEST(oatpp_case, request_response_test) {

	init_oatpp();

	{
		brick_uptr<xtree_t> server_xt(
			create_xtree_from_xml(
				"<bricks>"
				" <oatpp>"
				"  <server name=\"server1\" hostname=\"127.0.0.1\" port=\"7858\" ipversion=\"4\">"
				"   <endpoints>"
				"    <endpoint method=\"GET\" path=\"api/{item-id}\"/>"
				"   </endpoints>"
				"  </server>"
				"  <client name=\"client1\" hostname=\"127.0.0.1\" port=\"7858\" method=\"GET\"  path=\"api/{item-id}\">"
				"   <headers>"
				"    <header name=\"Accept-Language\" value=\"en-US,en;q=0.5\"/>"
				"    <header name=\"Accept-Encoding\" value=\"gzip, deflate, br\"/>"
				"   </headers>"
				"  </client>"
				" </oatpp>"
				"</bricks>"
			));

		brick_uptr<xtree_t> client_send_xt(create_xtree_from_xml(
			"<bricks>"
			" <oatpp>"
			"  <request>"
			"   <path_parameters>"
			"    <parameter name=\"item-id\" value=\"555\"/>"
			"   </path_parameters>"
			"   <query_parameters>"
			"    <parameter name=\"color\" value=\"orange\"/>"
			"   </query_parameters>"
			"  </request>"
			" </oatpp>"
			"</bricks>"
		));

		brick_uptr<xtree_t> server_response_xt(create_xtree_from_xml(
			"<bricks>"
			" <oatpp>"
			"  <response>"
			"   <headers>"
			"    <header name=\"Age\" value=\"10\"/>"
			"   </headers>"
			"  </response>"
			" </oatpp>"
			"</bricks>"
		));

		brick_uptr<server_plugin_t>  server(create_oatpp_http_server());

		brick_uptr<client_plugin_t>  client(create_oatpp_http_client());

		brick_uptr<cb_queue_t>  cb_q(create_callback_queue());

		brick_uptr<selector_t>  selector(create_selector());

		selector->init(cb_q.get());

		request_response_test_1(server_xt.get(), server.get(), server_xt.get(), client.get(), selector.get(), client_send_xt.get(), server_response_xt.get());
	}

	destroy_oatpp();
	

}

#endif