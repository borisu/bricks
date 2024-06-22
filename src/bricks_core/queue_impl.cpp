#include "pch.h"
#include "queue_impl.h"


using namespace bricks;


bricks_error_code_e
cb_queue_impl_t::enqueue(callback_t cb, int* event_id)
{
	int c = ++counter;
	if (event_id != nullptr)
		*event_id = c;
	
	
	cb_queue.push({ c,cb });

	return BRICKS_SUCCESS;
}

bricks_error_code_e
cb_queue_impl_t::try_dequeue(callback_t& cb, int milliseconds, int* event_id)
{
	cb_ctx_t v;
	if (!cb_queue.tryWaitAndPop(v, milliseconds))
	{
		return BRICKS_TIMEOUT;
	}

	if (event_id != nullptr)
		*event_id = v.counter;

	cb = v.cb;

	return BRICKS_SUCCESS;
}

cb_queue_t*
bricks::create_callback_queue()
{
	return new cb_queue_impl_t();
}
