#pragma once
#include "queue.h"

using namespace std;
using namespace bricks;
using namespace sw::redis;


namespace bricks::plugins {

	class redispp_subscriber_t : 
		public subscriber_plugin_t
	{
	public:

		redispp_subscriber_t();

		virtual bricks_error_code_e init(cb_queue_t* queue, topic_cb_t msg_cb, const xtree_t* options ) override;

		virtual bricks_error_code_e subscribe(const string& topic, const xtree_t* options ) override;

		virtual bricks_error_code_e unsubscribe(const string& topic, const xtree_t* options ) override;

		virtual bool check_capability(plugin_capabilities_e) override;

		virtual void release() override { delete this; };

		virtual ~redispp_subscriber_t();

		virtual void set_meta_cb(meta_cb_t) override {};

		void destroy();

	protected:

		virtual void on_topic(std::string channel, std::string msg);

		virtual void on_meta(Subscriber::MsgType type, OptionalString channel, long long num);

		virtual void on_error(std::exception_ptr e);

		cb_queue_t* queue = nullptr;

		string name;

		string url;

		bool initiated = false;

		bool destroyed = false;

		std::mutex mtx;

		ConnectionOptions opts;

		topic_cb_t msg_cb;

		AsyncRedis* redis = nullptr;

		AsyncSubscriber *subscriber = nullptr;

	};
}

