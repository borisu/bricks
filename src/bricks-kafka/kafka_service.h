#pragma once

#define ASSERT_INITIATED if (!this->initiated) return BRICKS_INVALID_STATE
#define ASSERT_NOT_INITIATED if (this->initiated) return BRICKS_INVALID_STATE

class kafka_service_t : public service_t
{
	virtual bricks_error_code_e poll(size_t timeout) override;

protected:

	kafka_service_t();

	bool initiated = false;

	rd_kafka_t* rd_kafka_h = nullptr;

	bricks_error_code_e init_conf(rd_kafka_conf_t* conf, const xtree_t* options);

	static void	kafka_logger(const rd_kafka_t* rk, int level, const char* fac, const char* buf);
	

};

