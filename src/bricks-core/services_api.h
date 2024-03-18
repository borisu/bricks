#pragma once
#include <functional>
#include "bricks_api.h"
#include "xtree_api.h"

using namespace std;

class polling_service_t
{
public:
	virtual bricks_error_code_e  poll(size_t timeout) = 0;

};


typedef
function<void(void*, bricks_error_code_e, xtree_t&)> delivery_cb_t;

class publisher_service_t : public polling_service_t
{
public:

	virtual bricks_error_code_e init(delivery_cb_t msg_cb, const xtree_t* options = nullptr) = 0;

	virtual bricks_error_code_e register_topic(const string& topic, const xtree_t* options = nullptr) = 0;

	virtual bricks_error_code_e publish(const string& topic, const buffer_t& buf, void* opaque, const xtree_t* options = nullptr) = 0;

};

typedef
function<void(void*, const char*, size_t, xtree_t&)> msg_cb_t;

class subscriber_service_t : public polling_service_t
{
public:

	virtual bricks_error_code_e init(msg_cb_t msg_cb, const xtree_t* options = nullptr) = 0;

	virtual bricks_error_code_e register_topic(const string& topic, const xtree_t* options = nullptr) = 0;

	virtual bricks_error_code_e subscribe(void *opaque, const xtree_t* options = nullptr) = 0;

};

typedef
function<void(void*, bricks_handle_t , const char*, size_t, const xtree_t&)> request_cb_t;

class server_service_t : public polling_service_t
{
public:

	virtual bricks_error_code_e init(const xtree_t* options = nullptr) = 0;

	virtual bricks_error_code_e register_request_handler(void *opaque, request_cb_t request, const xtree_t* options = nullptr) = 0;

	virtual bricks_error_code_e send_response(bricks_handle_t ctx, const char*, size_t, const xtree_t* options = nullptr) = 0;

};

typedef
function<void(void*, const char*, size_t, xtree_t&)> response_cb_t;

class client_service_t : public polling_service_t
{
public:

	virtual bricks_error_code_e init(const xtree_t* options = nullptr) = 0;

	virtual bricks_error_code_e register_client(void *opaque, response_cb_t rsp_cb, const xtree_t* options = nullptr) = 0;

	virtual bricks_error_code_e issue_request(const char*, size_t, const xtree_t* options = nullptr) = 0;
};






