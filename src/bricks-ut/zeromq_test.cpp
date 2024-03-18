#include "pch.h"
#include "gtest/gtest.h"
#include "zeromq_api.h"

TEST(zeromq_case, publish_subscribe_test) {

	/* prepare producer */
	const char* producer_config_xml =
		"<configuration>"
		" <publisher url = \"tcp://localhost:5555\"/>"
		"</configuration>";
	publisher_service_t* producer = create_zeromq_publisher();
	auto producer_config = create_xtree();
	ASSERT_EQ(BRICKS_SUCCESS, producer_config->load_from_xml(producer_config_xml));
	int delivered_counter = 0;
	ASSERT_EQ(BRICKS_SUCCESS, producer->init([&delivered_counter](void*, bricks_error_code_e, xtree_t&)
		{
			printf("SENT\n");
			delivered_counter++;
		}, producer_config));
	ASSERT_EQ(BRICKS_SUCCESS, producer->register_topic("bricks.test"));


	/* prepare consumer */
	const char* consumer_config_xml =
		"<configuration>"
		" <subscriber url = \"tcp://localhost:5555\"/>"
		"</configuration>";
	subscriber_service_t* consumer = create_zeromq_subscriber();
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
	buffer_t buf(msg, msg + strlen(msg));

	ASSERT_EQ(BRICKS_SUCCESS, consumer->subscribe(nullptr));
	consumer->poll(1000);

	ASSERT_EQ(BRICKS_SUCCESS, producer->publish("bricks.test", buf, nullptr));

	int poll_counter = 0;
	while (poll_counter < 10 && received_counter == 0)
	{
		consumer->poll(1000);
		producer->poll(1000);
	}

	destroy_zeromq_publisher(producer);
	destroy_zeromq_subscriber(consumer);

	destroy_xtree(producer_config);
	destroy_xtree(consumer_conf);
  
}

TEST(zeromq_case, request_reply_test) {

	/* prepare producer */
	auto server_config_xml =
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
		nullptr));


	/* prepare consumer */
	auto client_config_xml =
		"<configuration>"
		" <client url = \"tcp://localhost:5555\"/>"
		"</configuration>";

	auto client_config = create_xtree();
	auto client = create_zeromq_router_client();
	
	ASSERT_EQ(BRICKS_SUCCESS, client_config->load_from_xml(client_config_xml));

	int received_counter = 0;

	ASSERT_EQ(BRICKS_SUCCESS, client->init(client_config));
	ASSERT_EQ(BRICKS_SUCCESS, client->register_client(&received_counter, [](void* opaque, const char*, size_t, xtree_t&) {

		++*((int*)opaque);
			
	}, nullptr));

	const char *buf = "ping";
	ASSERT_EQ(BRICKS_SUCCESS, client->issue_request(buf, strlen(buf) + 1, nullptr));

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
	destroy_xtree(client_config);

}