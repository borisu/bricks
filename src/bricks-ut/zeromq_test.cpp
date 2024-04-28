#include "pch.h"
#include "gtest/gtest.h"
#include "zeromq_plugin.h"
#include "generic_tests.h"

using namespace bricks::plugins;

TEST(zeromq_case, bidi_test) {

	brick_uptr<xtree_t> xt1(
		create_xtree_from_xml(
			"<bricks>"
			"  <zmq name=\"oatpp_server1\">"
			"   <bidi url=\"tcp://127.0.0.1:7858\"is_server=\"true\"/>"
			"  </zmq>"
			"</bricks>"
		));

	brick_uptr<xtree_t> xt2(
		create_xtree_from_xml(
			"<bricks>"
			"  <zmq name=\"oatpp_server1\">"
			"   <bidi url=\"tcp://127.0.0.1:7858\" port=\"7858\" is_server=\"false\"/>"
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

	/* prepare producer */
	/*
	const char* producer_config_xml =
		"<configuration>"
		" <publisher url = \"tcp://localhost:5555\"/>"
		"</configuration>";
	publisher_plugin_t* producer = create_zeromq_publisher();
	auto producer_config = create_xtree();
	ASSERT_EQ(BRICKS_SUCCESS, producer_config->load_from_xml(producer_config_xml));
	int delivered_counter = 0;
	ASSERT_EQ(BRICKS_SUCCESS, producer->init([&delivered_counter](void*, bricks_error_code_e, xtree_t&)
		{
			printf("SENT\n");
			delivered_counter++;
		}, producer_config));
	ASSERT_EQ(BRICKS_SUCCESS, producer->register_topic("bricks.test"));*/


	/* prepare consumer */
	/*const char* consumer_config_xml =
		"<configuration>"
		" <subscriber url = \"tcp://localhost:5555\"/>"
		"</configuration>";
	subscriber_plugin_t* consumer = create_zeromq_subscriber();
	auto consumer_conf = create_xtree();
	ASSERT_EQ(BRICKS_SUCCESS, consumer_conf->load_from_xml(consumer_config_xml));
	int received_counter = 0;
	ASSERT_EQ(BRICKS_SUCCESS, consumer->init([&received_counter](void*, const char*, size_t, xtree_t&)
		{
			printf("RECEIVED\n");
			received_counter++;
		}, consumer_conf));
	ASSERT_EQ(BRICKS_SUCCESS, consumer->register_topic("bricks.test"));


	const char* msg = "Another brick in the wall.";
	

	ASSERT_EQ(BRICKS_SUCCESS, consumer->subscribe(nullptr));
	consumer->poll(1000);

	ASSERT_EQ(BRICKS_SUCCESS, producer->publish("bricks.test", msg, strlen(msg), nullptr));

	int poll_counter = 0;
	while (poll_counter < 10 && received_counter == 0)
	{
		consumer->poll(1000);
		producer->poll(1000);
	}

	destroy_zeromq_publisher(producer);
	destroy_zeromq_subscriber(consumer);

	destroy_xtree(producer_config);
	destroy_xtree(consumer_conf);*/
  
}

TEST(zeromq_case, request_reply_test) {

	/* prepare producer */
	/* auto server_config_xml =
		"<configuration>"
		" <server url = \"tcp://localhost:5555\"/>"
		"</configuration>";
	auto server			= create_zeromq_router_server();
	auto server_config	= create_xtree();
	ASSERT_EQ(BRICKS_SUCCESS, server_config->load_from_xml(server_config_xml));
	ASSERT_EQ(BRICKS_SUCCESS, server->init(server_config));

	
	int delivered_counter = 0;
	
	ASSERT_EQ(BRICKS_SUCCESS, server->register_request_handler(
		&delivered_counter,
		[server](void* opaque, bricks_handle_t ctx, const char*, size_t, const xtree_t&)
		{
			const char* req_str = "pong";
			++*((int*)opaque);
			server->send_response(ctx, req_str, strlen(req_str) + 1, nullptr);

		}, 
		nullptr));*/


	/* prepare consumer */
	/*auto client_config_xml =
		"<configuration>"
		" <client url = \"tcp://localhost:5555\"/>"
		"</configuration>";

	auto client_config = create_xtree();
	auto client = create_zeromq_dealer_bidi();
	
	ASSERT_EQ(BRICKS_SUCCESS, client_config->load_from_xml(client_config_xml));

	int received_counter = 0;

	ASSERT_EQ(BRICKS_SUCCESS, client->init(client_config));
	ASSERT_EQ(BRICKS_SUCCESS, client->register_event_handler(&received_counter, [](void* opaque, const char*, size_t, xtree_t&) {

		++*((int*)opaque);
			
	}, nullptr));

	const char *buf = "ping";
	ASSERT_EQ(BRICKS_SUCCESS, client->send_event(buf, strlen(buf) + 1, nullptr));

	int poll_counter = 0;
	while (poll_counter < 2 &&  received_counter == 0 )
	{
		poll_counter++;
		client->poll(1000);
		server->poll(1000);
	}

	ASSERT_EQ(1, received_counter);
	destroy_zeromq_router_client(client);
	destroy_zeromq_router_server(server);


	destroy_xtree(server_config);
	destroy_xtree(client_config);*/

}