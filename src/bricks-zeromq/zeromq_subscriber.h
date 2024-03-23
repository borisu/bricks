#pragma once
#include "zeromq_service.h"

using namespace std;

class zeromq_subscriber_t:
	public zeromq_service_t,
	public subscriber_plugin_t
{
public:

	zeromq_subscriber_t();

	virtual bricks_error_code_e init(msg_cb_t msg_cb, const xtree_t* options)  override;

	virtual bricks_error_code_e register_topic(const string& topic, const xtree_t* options) override;

	virtual bricks_error_code_e subscribe(void* opaque, const xtree_t* options) override;

	virtual bricks_error_code_e poll(size_t timeout) override;

	virtual ~zeromq_subscriber_t();

private:

	void destroy();

	msg_cb_t msg_cb;

	void* context = nullptr;

	void* subscriber = nullptr;

	zmq_pollitem_t items[1] = { 0 };

	void* opaque = nullptr;

	string url;

};

