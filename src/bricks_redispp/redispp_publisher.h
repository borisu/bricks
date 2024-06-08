#pragma once
#include "queue.h"

using namespace std;
using namespace bricks;
using namespace sw::redis;


namespace bricks::plugins {

	class redispp_publisher_t : 
		public publisher_plugin_t
	{
	public:

		redispp_publisher_t();

		virtual bricks_error_code_e init(cb_queue_t* queue, const xtree_t* options) override;

		virtual bricks_error_code_e add_topic(const string& topic, const xtree_t* options) override;

		virtual bricks_error_code_e publish(const string& topic, const char*, size_t, const xtree_t* options)  override;

		virtual bricks_error_code_e start()  override;

		virtual bool check_capability(plugin_capabilities_e) override;

		virtual void release() override { delete this; };

		virtual ~redispp_publisher_t();

		void destroy();

	protected:

		cb_queue_t* queue = nullptr;

		string name;

		string url;

		bool initiated = false;

		bool started = false;

		std::recursive_mutex mtx;

		ConnectionOptions opts;

		AsyncRedis* redis = nullptr;

	};
}

