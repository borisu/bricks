#include "pch.h"

#ifndef NO_lIBEVENT_PLUGIN

#include "generic_tests.h"
#include "libevent_plugin.h"

using namespace std;
using namespace bricks;
using namespace bricks::plugins;

TEST(libevent_case, request_response_test) {

    ASSERT_EQ(BRICKS_SUCCESS, libevent_init());

    for (int i = 0; i < NUM_OF_TESTS; i++) {

        brick_uptr<xtree_t> options_xt(
            create_xtree_from_xml(
                "<bricks>"
                " <libevent>"
                "  <server name=\"libevent_server1\" debug=\"true\">"
                "   <methods>"
                "    <init>"
                "     <connection address=\"127.0.0.1\" port=\"8080\"/>"
                "    </init>"
                "   </methods>"
                "  </server>"
                "  <client name=\"libevent_client1\" debug=\"true\">"
                "   <methods>"
                "    <init>"		
                "	   <connection url=\"127.0.0.1\" port=\"5672\"/>"
                "    </init>"
                "   </methods>"
                "  </client>"
                " </libevent>"
                "</bricks>"
            )
        );
        
        brick_uptr<server_plugin_t>  server(create_libevent_server());

        brick_uptr<client_plugin_t>  client(create_libevent_client());

        brick_uptr<cb_queue_t>  cb_q(create_callback_queue());

        brick_uptr<selector_t>  selector(create_selector());

        selector->init(cb_q.get());
       
        request_response_test_2(server.get(), client.get(), selector.get(), options_xt.get());

        libevent_destroy();
    }

}

#endif