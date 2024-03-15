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