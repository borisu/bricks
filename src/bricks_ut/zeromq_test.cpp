#include "pch.h"
#include "gtest/gtest.h"
#include "zeromq_plugin.h"
#include "generic_tests.h"

using namespace bricks::plugins;

TEST(zeromq_case, bidi_test) {

	brick_uptr<xtree_t> xt1(
		create_xtree_from_xml(
			"<bricks>"
			"  <zmq>"
			"	<bidi name=\"zmq_p1\" url=\"tcp://127.0.0.1:7858\" is_server=\"true\">"
			"	 <sockopt id=\"ZMQ_BACKLOG\" value=\"100\" />" 
			"	 <sockopt id=\"ZMQ_MAXMSGSIZE\" value=\"%L:-1\" />" 
			"	</bidi>"
			"  </zmq>"
			"</bricks>"
		));

	brick_uptr<xtree_t> xt2(
		create_xtree_from_xml(
			"<bricks>"
			"  <zmq>"
			"   <bidi name=\"zmq_p2\" url=\"tcp://127.0.0.1:7858\" port=\"7858\" is_server=\"false\">"
			"   </bidi>"
			"  </zmq>"
			"</bricks>"
		));


	brick_uptr<bidi_plugin_t>  p1(create_zeromq_bidi());

	brick_uptr<bidi_plugin_t>  p2(create_zeromq_bidi());

	brick_uptr<cb_queue_t>  cb_q(create_callback_queue());

	brick_uptr<selector_t>  selector(create_selector());

	selector->init(cb_q.get());

	bidi_test_1(xt1.get(), p1.get(),xt2.get(), p2.get(), selector.get());

}

TEST(zeromq_case, publish_subscribe_test) {

	brick_uptr<xtree_t> p_xt (
		create_xtree_from_xml(
			"<bricks>"
			"  <zmq>"
			"   <publisher name=\"publisher1\" url=\"tcp://127.0.0.1:7858\" is_server=\"true\">"
			"	 <sockopt id=\"ZMQ_BACKLOG\" value=\"100\" />"
			"   </publisher>"
			"  </zmq>"
			"</bricks>"
			)
		);

	brick_uptr<xtree_t> s_xt(
		create_xtree_from_xml(
			"<bricks>"
			"  <zmq>"
			"   <subscriber name=\"subscriber1\" url=\"tcp://127.0.0.1:7858\" is_server=\"false\">"
			"	 <sockopt id=\"ZMQ_SUBSCRIBE\" value=\"some.other.topic\" />"
			"   </subscriber>"
			"  </zmq>"
			"</bricks>"
		));

	brick_uptr<publisher_plugin_t>  publisher(create_zeromq_publisher());
	
	brick_uptr<subscriber_plugin_t>  subscriber(create_zeromq_subscriber());

	brick_uptr<cb_queue_t>  cb_q(create_callback_queue());

	brick_uptr<selector_t>  selector(create_selector());

	selector->init(cb_q.get());
	
	publish_subscribe_test_1(p_xt.get(), publisher.get(), s_xt.get(), subscriber.get(), selector.get());
  
}

