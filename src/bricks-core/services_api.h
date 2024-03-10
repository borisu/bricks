#pragma once
#include "bricks_api.h"
#include "xtree_api.h"

enum bricks_service_type_e
{
	BRICKS_NONE			= 0x0,
	BRICKS_PUBLISH		= 0x1,
	BRICKS_SUBSCRIBE	= 0x1 << 1,
	BRICKS_REQUEST		= 0x1 << 2,
	BRICKS_RESPOND		= 0x1 << 3
};


typedef void (*msg_cb_t)
(bricks_error_code_e e, const char* msg);

class service_t
{
public:

	virtual bricks_error_code_e  init(const xtree_t *options = nullptr) = 0;

	virtual bricks_error_code_e  register_publisher(const string& topic, const xtree_t* options = nullptr) = 0;

	virtual bricks_error_code_e  unregister_publisher(const string& topic) = 0;

	virtual bricks_error_code_e  unsubscribe(const string& topic) = 0;

	virtual bricks_error_code_e  publish(const string& topic, const buffer_t &buf, void *opaque, const xtree_t *options = nullptr) = 0;

	virtual bricks_error_code_e  subscribe(const string& topic, void* opaque, const xtree_t* options = nullptr) = 0;

	virtual bricks_error_code_e  poll() = 0;

};






