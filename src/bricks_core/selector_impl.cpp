#include "pch.h"
#include "selector_impl.h"
#include "queue.h"


using namespace std;
using namespace bricks;

bricks_error_code_e 
selector_impl_t::init(cb_queue_t *queue)
{
	if (!queue)
		return BRICKS_INVALID_PARAM;

	this->q = queue;

	return BRICKS_SUCCESS;

}

cb_queue_t* selector_impl_t::queue()
{
	return this->q;
}

bricks_error_code_e 
selector_impl_t::poll(int milliseconds)
{
	if (!q)
		return BRICKS_INVALID_STATE;

	callback_t cb = nullptr;
	bricks_error_code_e err;
	if ((err = q->try_dequeue(cb, milliseconds)) != BRICKS_SUCCESS)
	{
		return BRICKS_TIMEOUT;
	}

	
	cb();
	
	
	return BRICKS_SUCCESS;
}


selector_t*
bricks::create_selector()
{
	return new selector_impl_t();
}

