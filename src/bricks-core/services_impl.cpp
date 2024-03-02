#include "pch.h"
#include "services_api.h"
#include "bricks_api.h"

#ifdef __cplusplus
extern "C" {
#endif

bricks_error_code_e
bricks_register_service_provider(const char* name, bricks_handle_t h)
{
	return BRICKS_SUCCESS;
}


bricks_error_code_e
bricks_publish(const char* service, const char* message)
{
	return BRICKS_SUCCESS;
}


#ifdef __cplusplus
}
#endif