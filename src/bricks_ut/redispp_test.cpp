#include "pch.h"

#ifndef NO_REDISPP_PLUGIN

#include "generic_tests.h"
#include "redispp_plugin.h"

using namespace std;
using namespace bricks;
using namespace bricks::plugins;

TEST(redispp_case, publish_subscribe_test) {

	for (int i = 0; i < 2; i++) {

		brick_uptr<xtree_t> p_xt(
			create_xtree_from_xml(
				"<bricks>"
				"  <redispp>"
				"   <publisher name=\"publisher1\" url=\"tcp://127.0.0.1:6379\" >"
				"   </publisher>"
				"  </redispp>"
				"</bricks>"
			)
		);

		brick_uptr<xtree_t> s_xt(
			create_xtree_from_xml(
				"<bricks>"
				"  <redispp>"
				"   <subscriber name=\"subscriber1\" url=\"tcp://127.0.0.1:6379\">"
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

#endif