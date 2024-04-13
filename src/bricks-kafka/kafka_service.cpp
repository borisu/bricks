#include "pch.h"
#include "kafka_service.h"

using namespace bricks;
using namespace bricks::plugins;

//https://github.com/confluentinc/librdkafka/blob/master/examples/rdkafka_example.c

#define KAFKA_POLL_TIMEOUT 500

void
kafka_service_t::rd_log(const rd_kafka_t* rk, int level, const char* fac, const char* buf) {

	bricks_debug_level_e log_level = BRICKS_DEBUG;

	kafka_service_t* THIS = (kafka_service_t*)rd_kafka_opaque(rk);

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


	log1(log_level, "%s - RDKAFKA-%i-%s: %s: %s\n",
		THIS->name.c_str(), fac,
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
		auto err = rd_poll(KAFKA_POLL_TIMEOUT, false);
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
		this->name = get_opt<string>(options->get_property_value("/rdkafka/plugin", "name"));
	
		size_t c = get_opt<size_t>(options->get_node_children_count("/rdkafka/configuration"),0);

		for (int i = 0; i < c && err == BRICKS_SUCCESS; i++)
		{
			auto prop = options->get_node_name(xp_t("/rdkafka/configuration", i)).value();
			if (prop != "property")
				continue;

			auto name  = get<string>(options->get_property_value(xp_t("/configuration", i), "name").value());
			auto value = options->get_property_value_as_string(xp_t("/configuration", i), "value").value();

			log1(BRICKS_DEBUG, "%s - (%d) Setting rd kafka property name=%s, value=%s\n", name.c_str(), i, name.c_str(), value.c_str());
			if (rd_kafka_conf_set(conf, name.c_str(), value.c_str(), nullptr, 0) != RD_KAFKA_CONF_OK)
			{
				log1(BRICKS_ALARM, "%s - Error: %s\n", name.c_str(), rd_kafka_err2str(rd_kafka_last_error()));
				err = BRICKS_3RD_PARTY_ERROR;
				throw std::exception();
			}
		}
	}
	catch (std::bad_optional_access&)
	{
		err = BRICKS_INVALID_PARAM;
	}
	catch (std::exception&)
	{

	}

	return err;

}

kafka_service_t::~kafka_service_t()
{
	stop_rd_poll_loop();
}

