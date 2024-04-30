#pragma once
#include "jester.h"
#include <map>

using namespace std;
using namespace bricks;

namespace bricks::plugins {

	class jester_subscriber_t;

	class jester_pubsub_ctx_t : public brick_t
	{
	public:

		bricks_error_code_e publish(const char* topic, const char* data, size_t size);

		bricks_error_code_e subscribe(const char* topic, jester_subscriber_t*);

		bricks_error_code_e unsubscribe(jester_subscriber_t*);

		virtual void release()  override { delete this; };

	protected:

		map<string, list< jester_subscriber_t*>>  subscribers;

	};


	class jester_publisher_t : public publisher_plugin_t
	{
	public:

		jester_publisher_t(jester_pubsub_ctx_t* ctx);

		virtual bricks_error_code_e init(cb_queue_t* queue, const xtree_t* options) override;

		virtual bricks_error_code_e register_topic(const string& topic, const xtree_t* options) override;

		virtual bricks_error_code_e publish(const string& topic, const char*, size_t, const xtree_t* options ) override;

		virtual bricks_error_code_e start() override;

		virtual void release()  override { delete this; };


	protected:

		bool initiated = false;

		bool started = false;

		jester_pubsub_ctx_t* ctx = nullptr;

		cb_queue_t* queue = nullptr;

	};


	class jester_subscriber_t : public subscriber_plugin_t
	{
	public:

		jester_subscriber_t(jester_pubsub_ctx_t* ctx);

		virtual bricks_error_code_e init(cb_queue_t* queue, msg_cb_t msg_cb, const xtree_t* options = nullptr) override;

		virtual bricks_error_code_e register_topic(const string& topic, const xtree_t* options = nullptr) override;

		virtual bricks_error_code_e on_received(const char *topic, const char* data, size_t size);

		virtual bricks_error_code_e start() override;

		virtual void release()  override { delete this; };

		virtual ~jester_subscriber_t();

	protected:

		bool initiated = false;

		bool started = false;

		msg_cb_t msg_cb;

		jester_pubsub_ctx_t* ctx = nullptr;

		cb_queue_t* queue = nullptr;

		list<string> topics;


	};
}

