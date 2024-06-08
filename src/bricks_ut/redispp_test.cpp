#include "pch.h"

#ifndef NO_REDISPP_PLUGIN

#include "generic_tests.h"
#include "redispp_plugin.h"


using namespace std;
using namespace bricks;
using namespace bricks::plugins;

TEST(redispp_case, publish_subscribe_test) {

	for (int i = 0; i < NUM_OF_TESTS; i++) {

		brick_uptr<xtree_t> p_xt(
			create_xtree_from_xml(
				"<bricks>"
				"  <redispp>"
				"   <publisher name=\"publisher1\" url=\"tcp://127.0.0.1:6379\"  >"
				"   </publisher>"
				"  </redispp>"
				"</bricks>"
			)
		);

		brick_uptr<xtree_t> s_xt(
			create_xtree_from_xml(
				"<bricks>"
				"  <redispp>"
				"   <subscriber name=\"subscriber1\" url=\"tcp://127.0.0.1:6379\" hierarchical=\"false\">"
				"   </subscriber>"
				"  </redispp>"
				"</bricks>"
			));

		brick_uptr<publisher_plugin_t>  publisher(create_redispp_publisher());

		brick_uptr<subscriber_plugin_t>  subscriber(create_redispp_subscriber());

		brick_uptr<cb_queue_t>  cb_q(create_callback_queue());

		brick_uptr<selector_t>  selector(create_selector());

		selector->init(cb_q.get());

		publish_subscribe_test_1(p_xt.get(), publisher.get(), s_xt.get(), subscriber.get(), selector.get());
	}


}


TEST(redispp_case, meta_request_response_test) {

	for (int i = 0; i < NUM_OF_TESTS; i++) {

		brick_uptr<xtree_t> client_p_xt(
			create_xtree_from_xml(
				"<bricks>"
				"  <redispp>"
				"   <publisher name=\"client-publisher1\" url=\"tcp://127.0.0.1:6379\" >"
				"   </publisher>"
				"   <subscriber name=\"client-subscriber1\" url=\"tcp://127.0.0.1:6379\">"
				"   </subscriber>"
				"  </redispp>"
				"</bricks>"
			)
		);

		brick_uptr<xtree_t> server_p_xt(
			create_xtree_from_xml(
				"<bricks>"
				"  <redispp>"
				"   <publisher name=\"server-publisher1\" url=\"tcp://127.0.0.1:6379\" >"
				"   </publisher>"
				"   <subscriber name=\"server-subscriber1\" url=\"tcp://127.0.0.1:6379\">"
				"   </subscriber>"
				"  </redispp>"
				"</bricks>"
			)
		);

		brick_uptr<publisher_plugin_t>  client_publisher(create_redispp_publisher());

		brick_uptr<subscriber_plugin_t>  client_subscriber(create_redispp_subscriber());

		brick_uptr<publisher_plugin_t>   server_publisher(create_redispp_publisher());

		brick_uptr<subscriber_plugin_t>  server_subscriber(create_redispp_psubscriber());

		brick_uptr<cb_queue_t>  cb_q(create_callback_queue());

		brick_uptr<selector_t>  selector(create_selector());

		selector->init(cb_q.get());

		brick_uptr<timer_t> timer(create_timer());

		brick_uptr<client_plugin_t>  client(create_pubsub_client(client_publisher.get(), client_subscriber.get(), timer.get(), "/bricks/meta/req", "/bricks/meta/rep", "/bricks/meta/err"));

		brick_uptr<server_plugin_t>  server(create_pubsub_server(server_publisher.get(), server_subscriber.get(), "/bricks/meta/req", "/bricks/meta/rep", "/bricks/meta/err"));

		request_response_test_1(server_p_xt.get(), server.get(), client_p_xt.get(), client.get(), selector.get());
	}


}
#endif