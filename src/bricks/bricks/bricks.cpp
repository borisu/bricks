#include "pch.h"
#include "bricks.h"
#include <typeinfo>

enum bricks_error_code_e bricks_open(const char* t, bricks_handle_t* h)
{
	printf("Hello World!");

	typedef decltype(&bricks_open) bricks_open_t;

	return BRICKS_SUCCESS;
};