#pragma once
#include "queue.h"
#include "locking_queue.h"

namespace bricks
{
	class cb_queue_impl_t : public cb_queue_t
	{
	public:

		virtual bricks_error_code_e enqueue(callback_t, int* event_id = nullptr) override;

		virtual bricks_error_code_e try_dequeue(callback_t&, int milliseconds, int* event_id = nullptr) override;

		virtual void release() override { delete this; };

	private:

		atomic<int> counter = 0;

		struct cb_ctx_t
		{
			int counter;

			callback_t cb;
		};

		LockingQueue<cb_ctx_t> cb_queue;

	};

	
}

