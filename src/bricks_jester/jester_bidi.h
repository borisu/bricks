#pragma once
#include "jester_plugin.h"
#include <map>

using namespace std;
using namespace bricks;

namespace bricks::plugins
{

	class jester_bidi_t;

	class jester_bidi_ctx_t : public brick_t
	{
	public:

		jester_bidi_ctx_t();

		bricks_error_code_e set_endpoint(int id, jester_bidi_t*);

		bricks_error_code_e send_event(int id, const char* data, size_t size);

		virtual void release()  override { delete this; };

		std::recursive_mutex mtx;

	protected:

		std::map<int, jester_bidi_t*>  endpoints;

	};

	class jester_bidi_t : public bidi_plugin_t
	{
	public:

		jester_bidi_t(jester_bidi_ctx_t* ctx, int id);

		virtual bricks_error_code_e init(cb_queue_t* queue, const xtree_t* options) override;

		virtual bricks_error_code_e register_event_cb(event_cb_t event_cb, const xtree_t* options) override;

		virtual bricks_error_code_e send_event(const char*, size_t, const xtree_t* options ) override;

		virtual bricks_error_code_e accept_event(const char*, size_t size);

		virtual bricks_error_code_e start() override;

		virtual bool check_capability(plugin_capabilities_e) override;

		virtual void release()  override { delete this; };

	private:

		jester_bidi_ctx_t* ctx;

		int id = -1;

		cb_queue_t* queue = nullptr;

		event_cb_t event_cb = nullptr;

		
	};

}



