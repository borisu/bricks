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
	int event_id = 0;
	if ((err = q->try_dequeue(cb, milliseconds, &event_id)) != BRICKS_SUCCESS)
	{
		return BRICKS_TIMEOUT;
	}

	try
	{
		cb();
	}
	catch (std::exception &e)
	{
		printf("selector_impl_t::poll exception - %s\n", e.what());
	};
	
	return BRICKS_SUCCESS;
}


selector_t*
bricks::create_selector()
{
	return new selector_impl_t();
}

