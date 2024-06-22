#pragma once

using namespace std;
using namespace bricks;

// https://github.com/alanxz/rabbitmq-c/blob/master/examples/utils.h

namespace bricks::plugins {

	bricks_error_code_e check_amqp_error(amqp_rpc_reply_t x, char const* name, char const* context);

	bricks_error_code_e check_error(int x, char const* name, char const* context);

}