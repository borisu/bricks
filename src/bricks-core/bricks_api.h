#pragma once
#include <cstdint>

#ifdef BRICKS_EXPORTS
	#define BRICKS_API __declspec(dllexport)
#else
	#define BRICKS_API __declspec(dllimport)
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define BRICKS_INTERFACE_VERSION "0.1.0"

typedef uintptr_t bricks_handle_t;
enum bricks_error_code_e
{
	BRICKS_SUCCESS         = 0,
	BRICKS_FAILURE_GENERIC = 1,
	BRICKS_INVALID_PARAM   = 2,
	BRICKS_INVALID_STATE   = 3
};

#ifdef __cplusplus
}
#endif
