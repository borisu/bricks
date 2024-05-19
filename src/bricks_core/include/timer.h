#pragma once
#include "common.h"
#include "xtree.h"
#include "queue.h"

namespace bricks
{

	typedef std::chrono::steady_clock clock_t;

	typedef clock_t::duration duration_t;

	typedef clock_t::time_point time_point_t;

	class BRICKS_API timer_t : public startable_brick_t
	{
	public:
		virtual bricks_error_code_e init(cb_queue_t* queue, const xtree_t* options = nullptr) = 0;

		virtual bricks_error_code_e schedule_timer(const callback_t& callback, const duration_t& delay, int &handle) = 0;

		virtual bricks_error_code_e cancel_timer(int& handle) = 0;
	};


	BRICKS_API timer_t* create_timer();
}

