#include "pch.h"
#include "zeromq_service.h"

using namespace bricks;
using namespace bricks::plugins;

map<string, int> option_names =
{
BRICKS_MAP_PAIR(ZMQ_AFFINITY),
BRICKS_MAP_PAIR(ZMQ_ROUTING_ID),
BRICKS_MAP_PAIR(ZMQ_SUBSCRIBE),
BRICKS_MAP_PAIR(ZMQ_UNSUBSCRIBE),
BRICKS_MAP_PAIR(ZMQ_RATE),
BRICKS_MAP_PAIR(ZMQ_RECOVERY_IVL),
BRICKS_MAP_PAIR(ZMQ_SNDBUF),
BRICKS_MAP_PAIR(ZMQ_RCVBUF),
BRICKS_MAP_PAIR(ZMQ_RCVMORE),
BRICKS_MAP_PAIR(ZMQ_FD),
BRICKS_MAP_PAIR(ZMQ_EVENTS),
BRICKS_MAP_PAIR(ZMQ_TYPE),
BRICKS_MAP_PAIR(ZMQ_LINGER),
BRICKS_MAP_PAIR(ZMQ_RECONNECT_IVL),
BRICKS_MAP_PAIR(ZMQ_BACKLOG),
BRICKS_MAP_PAIR(ZMQ_RECONNECT_IVL_MAX),
BRICKS_MAP_PAIR(ZMQ_MAXMSGSIZE),
BRICKS_MAP_PAIR(ZMQ_SNDHWM),
BRICKS_MAP_PAIR(ZMQ_RCVHWM),
BRICKS_MAP_PAIR(ZMQ_MULTICAST_HOPS),
BRICKS_MAP_PAIR(ZMQ_RCVTIMEO),
BRICKS_MAP_PAIR(ZMQ_SNDTIMEO),
BRICKS_MAP_PAIR(ZMQ_LAST_ENDPOINT),
BRICKS_MAP_PAIR(ZMQ_ROUTER_MANDATORY),
BRICKS_MAP_PAIR(ZMQ_TCP_KEEPALIVE),
BRICKS_MAP_PAIR(ZMQ_TCP_KEEPALIVE_CNT),
BRICKS_MAP_PAIR(ZMQ_TCP_KEEPALIVE_IDLE),
BRICKS_MAP_PAIR(ZMQ_TCP_KEEPALIVE_INTVL),
BRICKS_MAP_PAIR(ZMQ_IMMEDIATE),
BRICKS_MAP_PAIR(ZMQ_XPUB_VERBOSE),
BRICKS_MAP_PAIR(ZMQ_ROUTER_RAW),
BRICKS_MAP_PAIR(ZMQ_IPV6),
BRICKS_MAP_PAIR(ZMQ_MECHANISM),
BRICKS_MAP_PAIR(ZMQ_PLAIN_SERVER),
BRICKS_MAP_PAIR(ZMQ_PLAIN_USERNAME),
BRICKS_MAP_PAIR(ZMQ_PLAIN_PASSWORD),
BRICKS_MAP_PAIR(ZMQ_CURVE_SERVER),
BRICKS_MAP_PAIR(ZMQ_CURVE_PUBLICKEY),
BRICKS_MAP_PAIR(ZMQ_CURVE_SECRETKEY),
BRICKS_MAP_PAIR(ZMQ_CURVE_SERVERKEY),
BRICKS_MAP_PAIR(ZMQ_PROBE_ROUTER),
BRICKS_MAP_PAIR(ZMQ_REQ_CORRELATE),
BRICKS_MAP_PAIR(ZMQ_REQ_RELAXED),
BRICKS_MAP_PAIR(ZMQ_CONFLATE),
BRICKS_MAP_PAIR(ZMQ_ZAP_DOMAIN),
BRICKS_MAP_PAIR(ZMQ_ROUTER_HANDOVER),
BRICKS_MAP_PAIR(ZMQ_TOS),
BRICKS_MAP_PAIR(ZMQ_CONNECT_ROUTING_ID),
BRICKS_MAP_PAIR(ZMQ_GSSAPI_SERVER),
BRICKS_MAP_PAIR(ZMQ_GSSAPI_PRINCIPAL),
BRICKS_MAP_PAIR(ZMQ_GSSAPI_SERVICE_PRINCIPAL),
BRICKS_MAP_PAIR(ZMQ_GSSAPI_PLAINTEXT),
BRICKS_MAP_PAIR(ZMQ_HANDSHAKE_IVL),
BRICKS_MAP_PAIR(ZMQ_SOCKS_PROXY),
BRICKS_MAP_PAIR(ZMQ_XPUB_NODROP),
BRICKS_MAP_PAIR(ZMQ_BLOCKY),
BRICKS_MAP_PAIR(ZMQ_XPUB_MANUAL),
BRICKS_MAP_PAIR(ZMQ_XPUB_WELCOME_MSG),
BRICKS_MAP_PAIR(ZMQ_STREAM_NOTIFY),
BRICKS_MAP_PAIR(ZMQ_INVERT_MATCHING),
BRICKS_MAP_PAIR(ZMQ_HEARTBEAT_IVL),
BRICKS_MAP_PAIR(ZMQ_HEARTBEAT_TTL),
BRICKS_MAP_PAIR(ZMQ_HEARTBEAT_TIMEOUT),
BRICKS_MAP_PAIR(ZMQ_XPUB_VERBOSER),
BRICKS_MAP_PAIR(ZMQ_CONNECT_TIMEOUT),
BRICKS_MAP_PAIR(ZMQ_TCP_MAXRT),
BRICKS_MAP_PAIR(ZMQ_THREAD_SAFE),
BRICKS_MAP_PAIR(ZMQ_MULTICAST_MAXTPDU),
BRICKS_MAP_PAIR(ZMQ_VMCI_BUFFER_SIZE),
BRICKS_MAP_PAIR(ZMQ_VMCI_BUFFER_MIN_SIZE),
BRICKS_MAP_PAIR(ZMQ_VMCI_BUFFER_MAX_SIZE),
BRICKS_MAP_PAIR(ZMQ_VMCI_CONNECT_TIMEOUT),
BRICKS_MAP_PAIR(ZMQ_USE_FD),
BRICKS_MAP_PAIR(ZMQ_GSSAPI_PRINCIPAL_NAMETYPE),
BRICKS_MAP_PAIR(ZMQ_GSSAPI_SERVICE_PRINCIPAL_NAMETYPE),
BRICKS_MAP_PAIR(ZMQ_BINDTODEVICE)
};


zeromq_service_t::zeromq_service_t()
{
	
}

bricks_error_code_e
zeromq_service_t::start_zmq_poll_loop()
{
	ASSERT_NOT_STARTED;

	zmq_poll_thread = new thread(&zeromq_service_t::zmq_poll_loop, this);

	started = true;

	return BRICKS_SUCCESS;
}

void
zeromq_service_t::stop_zmq_poll_loop()
{
	if (zmq_poll_thread)
	{
		shutdown = true;
		zmq_poll_thread->join();
		zmq_poll_thread = nullptr;
	}
}

#define ZMQ_POLL_TIMEOUT 500

void
zeromq_service_t::zmq_poll_loop()
{
	while (shutdown != true)
	{
		auto err = do_zmq_poll(ZMQ_POLL_TIMEOUT, false);
		switch (err)
		{
		case BRICKS_TIMEOUT:
		case BRICKS_SUCCESS:
			continue;
		default:
			break;

		}
	}

	do_zmq_poll(ZMQ_POLL_TIMEOUT, true);

}

zeromq_service_t::~zeromq_service_t()
{
	stop_zmq_poll_loop();
}

bricks_error_code_e
zeromq_service_t::set_sockopt(const xtree_t* xt, const char* parent, void* sock)
{
	if (!xt)
		return BRICKS_SUCCESS;

	auto err = BRICKS_SUCCESS;

	try
	{
		size_t ocount = xt->get_node_children_count(parent).value();
		for (int i = 0; i < ocount; i++)
		{
			if (xt->get_node_name(xp_t(parent, i)).value() == "sockopt")
			{
				string name = get<string>(xt->get_property_value(xp_t(parent, i), "id").value());

				int id = option_names[name];

				auto val = xt->get_property_value(xp_t(parent, i), "value").value();
				if (holds_alternative<long long>(val))
				{
					long long opt = std::get<long long>(val);
					if (zmq_setsockopt(sock, id, &opt, sizeof(opt)) == -1) {
						BRICK_LOG_ZMQ_ERROR(zmq_setsockopt);
						err =  BRICKS_3RD_PARTY_ERROR;
						break;
					}
				}
				else if (holds_alternative<int>(val))
				{
					int opt = std::get<int>(val);
					if (zmq_setsockopt(sock, id, &opt, sizeof(opt)) == -1) {
						BRICK_LOG_ZMQ_ERROR(zmq_setsockopt);
						err = BRICKS_3RD_PARTY_ERROR;
						break;
					}
				}
				else if (holds_alternative<string>(val))
				{
					string opt = std::get<string>(val);
					if (zmq_setsockopt(sock, id, opt.c_str(), strlen(opt.c_str())) == -1) {
						BRICK_LOG_ZMQ_ERROR(zmq_setsockopt);
						err = BRICKS_3RD_PARTY_ERROR;
						break;
					}
				}
			}
		};

		return err;
	}
	catch (std::exception&)
	{
		return BRICKS_INVALID_PARAM;
	}
}
