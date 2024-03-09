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


typedef void (*msg_delivered_cb_t)
(bricks_error_code_e e, const char* msg);

class service_t
{
public:

	virtual bricks_error_code_e  init(const xtree_t &configuration) = 0;

};






