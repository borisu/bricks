#include "pch.h"

#ifndef NO_REDISPP_PLUGIN

#include "generic_tests.h"
#include "redispp_plugin.h"


using namespace std;
using namespace bricks;
using namespace bricks::plugins;

TEST(redispp_case, publish_subscribe_test) {

	for (int i = 0; i < NUM_OF_TESTS; i++) {

		brick_uptr<xtree_t> options_xt(
			create_xtree_from_xml(
				"<bricks>"
				"  <redispp>"
				"   <publisher name=\"redispp_publisher1\" >"
				"     <methods>"
				"     <init>"
				"		<connection url=\"tcp://127.0.0.1:6379\" />"	
				"     </init>"
				"    </methods>"
				"   </publisher>"
				"   <subscriber name=\"redispp_subscriber\" >"
				"     <methods>"
				"     <init>"
				"		<connection url=\"tcp://127.0.0.1:6379\" />"
				"     </init>"
				"    </methods>"
				"   </subscriber>"
				"  </redispp>"
				"</bricks>"
			)
		);

		brick_uptr<cb_queue_t>  cb_q(create_callback_queue());

		brick_uptr<selector_t>  selector(create_selector());

		selector->init(cb_q.get());

		brick_uptr<publisher_plugin_t>  publisher(create_redispp_publisher());

		brick_uptr<subscriber_plugin_t>  subscriber(create_redispp_subscriber());

		publish_subscribe_test_2(publisher.get(), subscriber.get(), selector.get(), options_xt.get());
	}


}


TEST(redispp_case, meta_request_response_test) {

	for (int i = 0; i < NUM_OF_TESTS; i++) {

		brick_uptr<xtree_t> client_options_xt(
			create_xtree_from_xml(
				"<bricks>"
				"  <redispp>"
				"   <publisher name=\"redispp_client_publisher1\" >"
				"     <methods>"
				"     <init>"
				"		<connection url=\"tcp://127.0.0.1:6379\" />"
				"     </init>"
				"    </methods>"
				"   </publisher>"
				"   <subscriber name=\"redispp_client_subscriber\" >"
				"     <methods>"
				"     <init>"
				"		<connection url=\"tcp://127.0.0.1:6379\" />"
				"     </init>"
				"    </methods>"
				"   </subscriber>"
				"  </redispp>"
				"</bricks>"
			)
		);

		brick_uptr<xtree_t> server_options_xt(
			create_xtree_from_xml(
				"<bricks>"
				"  <redispp>"
				"   <publisher name=\"redispp_server_publisher1\" >"
				"     <methods>"
				"     <init>"
				"		<connection url=\"tcp://127.0.0.1:6379\" />"
				"     </init>"
				"    </methods>"
				"   </publisher>"
				"   <subscriber name=\"redispp_server_subscriber\" >"
				"     <methods>"
				"     <init>"
				"		<connection url=\"tcp://127.0.0.1:6379\" />"
				"     </init>"
				"    </methods>"
				"   </subscriber>"
				"  </redispp>"
				"</bricks>"
			)
		);

		brick_uptr<cb_queue_t>  cb_q(create_callback_queue());

		brick_uptr<selector_t>  selector(create_selector());

		selector->init(cb_q.get());

		brick_uptr<timer_t> timer(create_timer());
		timer->init(cb_q.get());
		

		brick_uptr<publisher_plugin_t>  client_publisher(create_redispp_publisher());

		brick_uptr<subscriber_plugin_t>  client_subscriber(create_redispp_subscriber());

		brick_uptr<publisher_plugin_t>   server_publisher(create_redispp_publisher());

		brick_uptr<subscriber_plugin_t>  server_subscriber(create_redispp_psubscriber());

		brick_uptr<client_plugin_t>  client(create_pubsub_client(client_publisher.get(), client_subscriber.get(), timer.get(), "/bricks/meta/req", "/bricks/meta/rep", "/bricks/meta/err"));

		brick_uptr<server_plugin_t>  server(create_pubsub_server(server_publisher.get(), server_subscriber.get(), "/bricks/meta/req", "/bricks/meta/rep", "/bricks/meta/err"));

		request_response_test_2(server.get(), client.get(), selector.get(), server_options_xt.get(), client_options_xt.get(), nullptr);
	}


}
#endif