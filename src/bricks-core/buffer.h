#pragma once
#include "common.h"

namespace bricks
{
	class buffer_t : public brick_t
	{
	public:

		virtual int size() = 0;

		virtual const char* data() = 0;

	};

	BRICKS_API buffer_t* create_buffer(const char* data, int size);

}



