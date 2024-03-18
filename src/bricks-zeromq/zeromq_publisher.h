#pragma once
#include "zeromq_service.h"
using namespace std;


class zeromq_publisher_t :
	public zeromq_service_t,
	public publisher_service_t
{
public:

	zeromq_publisher_t();

	virtual bricks_error_code_e init(delivery_cb_t msg_cb, const xtree_t* options) override;

	virtual bricks_error_code_e register_topic(const string& topic, const xtree_t* options) override;

	virtual bricks_error_code_e publish(const string& topic, const buffer_t& buf, void* opaque, const xtree_t* options)  override;

	virtual bricks_error_code_e poll(size_t timeout) override;

	virtual ~zeromq_publisher_t();

private:

	void destroy();
	
	delivery_cb_t msg_cb = nullptr;

	void* context = nullptr;

	void* publisher = nullptr;

	string url;

	string topic;

};
