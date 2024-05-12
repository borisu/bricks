#include "pch.h"
#include "queue_impl.h"


using namespace bricks;


bricks_error_code_e
cb_queue_impl_t::enqueue(callback_t cb)
{
	cb_queue.push(cb);

	return BRICKS_SUCCESS;
}

bricks_error_code_e
cb_queue_impl_t::try_dequeue(callback_t&, int milliseconds)
{
	callback_t cb = nullptr;
	if (!cb_queue.tryWaitAndPop(cb, milliseconds))
	{
		return BRICKS_TIMEOUT;
	}

	try
	{
		cb();
	}
	catch (std::exception&)
	{

	}

	return BRICKS_SUCCESS;
}

cb_queue_t*
bricks::create_callback_queue()
{
	return new cb_queue_impl_t();
}
