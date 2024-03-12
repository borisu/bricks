#include "pch.h"
#include "kafka_service.h"


// https://github.com/confluentinc/librdkafka/blob/master/examples/rdkafka_example.c
void
kafka_service_t::kafka_logger(const rd_kafka_t* rk, int level, const char* fac, const char* buf) {

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

kafka_service_t::kafka_service_t() {};

bricks_error_code_e  
kafka_service_t::poll(size_t timeout)
{
	ASSERT_INITIATED;

	rd_kafka_poll(rd_kafka_h, timeout);
}

bricks_error_code_e
kafka_service_t::init_conf(rd_kafka_conf_t* conf, const xtree_t* options)
{
	bricks_error_code_e err = BRICKS_SUCCESS;

	try
	{
		int c = options->get_node_children_count("/configuration").value();


		for (int i = 0; i < c && err == BRICKS_SUCCESS; i++)
		{
			auto prop = options->get_child_name_by_index("/configuration", i).value();
			if (prop != "property")
				continue;

			auto name = options->get_child_property_value_as_string("/configuration", i, "name").value();
			auto value = options->get_child_property_value_as_string("/configuration", i, "value").value();

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