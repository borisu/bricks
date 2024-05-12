#pragma once
#include "queue.h"
#include "locking_queue.h"

namespace bricks
{
	class cb_queue_impl_t : public cb_queue_t
	{
	public:

		virtual bricks_error_code_e enqueue(callback_t);

		virtual bricks_error_code_e try_dequeue(callback_t&, int milliseconds);

		virtual void release() override { delete this; };

	private:

		LockingQueue<callback_t> cb_queue;

	};

	
}

