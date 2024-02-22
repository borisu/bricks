#pragma once
#ifdef BRICKS_EXPORTS
#define BRICKS_API __declspec(dllexport)
#else
#define BRICKS_API __declspec(dllimport)
#endif

#ifdef __cplusplus
extern "C" {
#endif

BRICKS_API void bricks_register_service();

#ifdef __cplusplus
}
#endif
