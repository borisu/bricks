#include "pch.h"
#include "generic_tests.h"

using namespace std;
using namespace bricks;
using namespace bricks::plugins;

TEST(oatpp_case, request_response_test) {

	init_oatpp();

	{
		brick_uptr<xtree_t> s_xt(
			create_xtree_from_xml(
				"<bricks>"
				"  <oatpp_http_server name=\"oatpp_server1\">"
				"   <server hostname=\"127.0.0.1\" port=\"7858\" ipversion=\"4\"/>"
				"   <endpoints>"
				"    <endpoint method=\"GET\" path=\"/api\"/>"
				"   </endpoints>"
				"  </oatpp_http_server>"
				"  <oatpp_http_client name=\"oatpp_server1\">"
				"   <client hostname=\"127.0.0.1\" port=\"7858\" method=\"GET\"  path=\"/api\" />"
				"  </oatpp_http_client>"
				"</bricks>"
			));

		brick_uptr<server_plugin_t>  server(create_oatpp_http_server());

		brick_uptr<client_plugin_t>  client(create_oatpp_http_client());

		brick_uptr<cb_queue_t>  cb_q(create_callback_queue());

		brick_uptr<selector_t>  selector(create_selector());

		selector->init(cb_q.get());

		request_response_test_1(s_xt.get(), server.get(), s_xt.get(), client.get(), selector.get());
	}

	destroy_oatpp();
	

}