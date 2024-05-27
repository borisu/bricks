#pragma once
#include "queue.h"
#include "common.h"

namespace bricks
{
	class selector_t : public brick_t
	{
	public:

		virtual bricks_error_code_e init(cb_queue_t *queue) = 0;

		virtual cb_queue_t* queue() = 0;

		virtual bricks_error_code_e poll(int milliseconds) = 0;

	};

	class poller_t : public brick_t
	{
	public:
	};

	BRICKS_API selector_t*
	create_selector();

	BRICKS_API poller_t*
	create_poller(int milliseconds, selector_t*);

}
