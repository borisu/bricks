#include "pch.h"
#include "rabbitmq_utils.h"

using namespace std;
using namespace bricks;
using namespace bricks::plugins;

bricks_error_code_e  
bricks::plugins::check_amqp_error(amqp_rpc_reply_t x, char const* name, char const* context)
{
    switch (x.reply_type) {
    case AMQP_RESPONSE_NORMAL:
        return BRICKS_SUCCESS;

    case AMQP_RESPONSE_NONE:
        log1(BRICKS_ALARM, "%s %%%%%% %s:missing RPC reply type!", name, context);
        break;

    case AMQP_RESPONSE_LIBRARY_EXCEPTION:
        log1(BRICKS_ALARM, "%s %%%%%% %s:%s", name, context, amqp_error_string2(x.library_error));
        break;

    case AMQP_RESPONSE_SERVER_EXCEPTION:
        switch (x.reply.id) {
        case AMQP_CONNECTION_CLOSE_METHOD: {
            amqp_connection_close_t* m =
                (amqp_connection_close_t*)x.reply.decoded;
            log1(BRICKS_ALARM, "%s %%%%%% %s: server connection error %uh, message: %.*s\n",
                name, context, m->reply_code, (int)m->reply_text.len,
                (char*)m->reply_text.bytes);
            break;
        }
        case AMQP_CHANNEL_CLOSE_METHOD: {
            amqp_channel_close_t* m = (amqp_channel_close_t*)x.reply.decoded;
            log1(BRICKS_ALARM, "%s %%%%%% %s:server channel error %uh, message: %.*s\n",
                name, context, m->reply_code, (int)m->reply_text.len,
                (char*)m->reply_text.bytes);
            break;
        }
        default:
            log1(BRICKS_ALARM, "%s %%%%%% %s:unknown server error, method id 0x%08X\n",
                name, context, x.reply.id);
            break;
        }
        break;
    }

    return BRICKS_3RD_PARTY_ERROR;
}

bricks_error_code_e 
bricks::plugins::check_error(int x, char const* name, char const* context)
{
    if (x < 0) {
        log1(BRICKS_ALARM, "%s %%%%%% %s: %s\n", name, context, amqp_error_string2(x));
        return BRICKS_3RD_PARTY_ERROR;
    };

    return BRICKS_SUCCESS;
}