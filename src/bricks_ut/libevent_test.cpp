#include "pch.h"

#ifndef NO_lIBEVENT_PLUGIN

#include "generic_tests.h"
#include "libevent_plugin.h"

using namespace std;
using namespace bricks;
using namespace bricks::plugins;

TEST(libevent_case, publish_subscribe_test) {

    for (int i = 0; i < NUM_OF_TESTS; i++) {

        brick_uptr<xtree_t> options_xt(
            create_xtree_from_xml(
                "<bricks>"
                "  <libevent>"
                "   <publisher name=\"publisher1\">"
                "    <methods>"
                "     <init>"
                "	    <connection hostname=\"127.0.0.1\" port=\"5672\">"
                "	     <login method=\"plain\" username=\"guest\" password=\"guest\" />"
                "	    </connection>"
                "     </init>"
                "     <describe_topic>"
                "	   <exchange declare=\"true\" type=\"topic\"/>"
                "     </describe_topic>"
                "    </methods>"
                "   </publisher>"
                "   <subscriber name=\"subscriber1\">"
                "   <methods>"
                "    <init>"		
                "	  <connection hostname=\"127.0.0.1\" port=\"5672\">"
                "	   <login method=\"plain\" username=\"guest\" password=\"guest\"/>"
                "	  </connection>"
                "	  <queue name=\"\" declare=\"true\" />"
                "    </init>"
                "    <subscribe>"
                "	  <exchange declare=\"true\" type=\"topic\"/>"
                "	  <queue name=\"\" declare=\"true\" />"
                "	  <bind name=\"\" declare=\"true\" />"
                "    </subscribe>"
                "   </methods>"
                "  </subscriber>"
                " </libevent>"
                "</bricks>"
            )
        );
        
        brick_uptr<publisher_plugin_t>  publisher(create_libevent_server());

        brick_uptr<subscriber_plugin_t>  subscriber(create_libevent_client());

        brick_uptr<cb_queue_t>  cb_q(create_callback_queue());

        brick_uptr<selector_t>  selector(create_selector());

        selector->init(cb_q.get());

        publish_subscribe_test_2(publisher.get(), subscriber.get(), selector.get(), options_xt.get());
    }

}

#endif