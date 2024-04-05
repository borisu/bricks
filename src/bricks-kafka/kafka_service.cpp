#include "pch.h"
#include "kafka_service.h"

using namespace bricks;
using namespace bricks::plugins;

//https://github.com/confluentinc/librdkafka/blob/master/examples/rdkafka_example.c

#define KAFKA_POLL_TIMEOUT 500

void
kafka_service_t::rd_log(const rd_kafka_t* rk, int level, const char* fac, const char* buf) {

	bricks_debug_level_e log_level = BRICKS_DEBUG;

	switch (level)
	{
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	case 6: { log_level = BRICKS_INFO; break; }
	default: { log_level = BRICKS_DEBUG; break; };
	};


	log1(log_level, "RDKAFKA-%i-%s: %s: %s\n"
		, fac,
		rk ? rd_kafka_name(rk) : NULL, buf);
}

kafka_service_t::kafka_service_t() 
{
	
};


bricks_error_code_e
kafka_service_t::start_rd_poll_loop()
{
	ASSERT_NOT_STARTED;

	rd_poll_thread = new thread(&kafka_service_t::rd_poll_loop, this);

	return BRICKS_SUCCESS;
}

void
kafka_service_t::stop_rd_poll_loop()
{
	if (rd_poll_thread)
	{
		shutdown = true;
		rd_poll_thread->join();
		rd_poll_thread = nullptr;
	}
}

void
kafka_service_t::rd_poll_loop()
{
	while (shutdown != true)
	{
		auto err = rd_poll(KAFKA_POLL_TIMEOUT,false);
		switch (err)
		{
		case BRICKS_TIMEOUT:
		case BRICKS_SUCCESS:
			continue;
		default:
			break;

		}
	}

	rd_poll(KAFKA_POLL_TIMEOUT, true);
}

bricks_error_code_e
kafka_service_t::init_conf(rd_kafka_conf_t* conf, const xtree_t* options)
{
	bricks_error_code_e err = BRICKS_SUCCESS;

	try
	{
		size_t c = options->get_node_children_count("/configuration").value();

		for (int i = 0; i < c && err == BRICKS_SUCCESS; i++)
		{
			auto prop = options->get_node_name(xp_t("/configuration", i)).value();
			if (prop != "property")
				continue;

			auto name  = get<string>(options->get_property_value(xp_t("/configuration", i), "name").value());
			auto value = get<string>(options->get_property_value(xp_t("/configuration", i), "value").value());

			char* errstr = nullptr;
			if (rd_kafka_conf_set(conf, name.c_str(), value.c_str(), errstr, sizeof(errstr)) != RD_KAFKA_CONF_OK)
			{
				log1(BRICKS_ALARM, "%% %s\n", errstr);
				err = BRICKS_3RD_PARTY_ERROR;
				break;
			}
		}
	}
	catch (std::bad_optional_access&)
	{
		err = BRICKS_INVALID_PARAM;
	}

	return err;

}

kafka_service_t::~kafka_service_t()
{
	stop_rd_poll_loop();
}

