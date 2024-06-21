#include "pch.h"
#include "gtest/gtest.h"
#include "zeromq_plugin.h"
#include "generic_tests.h"

using namespace bricks::plugins;

TEST(zeromq_case, bidi_test) {

    for (int i = 0; i < 2; i++)
    {

        brick_uptr<xtree_t> xt1(
            create_xtree_from_xml(
                "<bricks>"
                "  <zmq>"
                "	<bidi name=\"zmq_p1\">"
                "    <methods>"
                "     <init>"
                "      <socket url=\"tcp://127.0.0.1:7858\" is_server=\"true\">"
                "	    <sockopt id=\"ZMQ_BACKLOG\" value=\"100\" />"
                "	    <sockopt id=\"ZMQ_MAXMSGSIZE\" value=\"%L:-1\" />"
                "      </socket>"
                "     </init>"
                "    </methods>"
                "   </bidi>"
                "  </zmq>"
                "</bricks>"
            ));

        brick_uptr<xtree_t> xt2(
            create_xtree_from_xml(
                "<bricks>"
                "  <zmq>"
                "	<bidi name=\"zmq_p2\">"
                "    <methods>"
                "    <init>"
                "     <socket url=\"tcp://127.0.0.1:7858\" is_server=\"false\">"
                "	   <sockopt id=\"ZMQ_BACKLOG\" value=\"100\" />"
                "	   <sockopt id=\"ZMQ_MAXMSGSIZE\" value=\"%L:-1\" />"
                "     </socket>"
                "    </init>"
                "   </methods>"
                "  </bidi>"
                " </zmq>"
                "</bricks>"
            ));


        brick_uptr<bidi_plugin_t>  p1(create_zeromq_bidi());

        brick_uptr<bidi_plugin_t>  p2(create_zeromq_bidi());

        brick_uptr<cb_queue_t>  cb_q(create_callback_queue());

        brick_uptr<selector_t>  selector(create_selector());

        selector->init(cb_q.get());

        bidi_test_2(p1.get(), p2.get(), selector.get(), xt1.get(), xt2.get());
    }

}

TEST(zeromq_case, publish_subscribe_test) {

    for (int i = 0; i < 2; i++) {

        brick_uptr<xtree_t> options_xt(
            create_xtree_from_xml(
                "<bricks>"
                " <zmq>"
                "  <publisher name=\"zmq_publisher1\">"
                "   <methods>"
                "    <init>"
                "     <socket  url=\"tcp://127.0.0.1:7858\" is_server=\"true\">"
                "      <sockopt id=\"ZMQ_BACKLOG\" value=\"100\" />"
                "     </socket>"
                "    </init>"
                "   </methods>"
                "  </publisher>"
                "  <subscriber name=\"zmq_subscriber1\">"
                "   <methods>"
                "    <init>"
                "     <socket url=\"tcp://127.0.0.1:7858\" is_server=\"false\" >"
                "      <sockopt id=\"ZMQ_SUBSCRIBE\" value=\"some.other.topic\" />"
                "     </socket>"
                "    </init>"
                "   </methods>"
                "  </subscriber>"
                " </zmq>"
                "</bricks>"
            )
        );

        brick_uptr<cb_queue_t>  cb_q(create_callback_queue());

        brick_uptr<selector_t>  selector(create_selector());

        selector->init(cb_q.get());

        brick_uptr<publisher_plugin_t>  publisher(create_zeromq_publisher());

        brick_uptr<subscriber_plugin_t>  subscriber(create_zeromq_subscriber());

        publish_subscribe_test_2(publisher.get(), subscriber.get(), selector.get(), options_xt.get());
    }
  
}

