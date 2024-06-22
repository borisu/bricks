#pragma once
#include "common.h"

namespace bricks
{
	typedef function<void()> callback_t;

	class cb_queue_t : public brick_t
	{
	public:

		virtual bricks_error_code_e enqueue(callback_t,  int* event_id = nullptr) = 0;

		virtual bricks_error_code_e try_dequeue(callback_t&, int milliseconds, int* event_id = nullptr) = 0;

	};

	BRICKS_API cb_queue_t*
	create_callback_queue();
	
}
