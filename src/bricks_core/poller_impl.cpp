#include "pch.h"
#include "poller_impl.h"

using namespace std;
using namespace bricks;

BRICKS_API poller_t*
bricks::create_poller(int milliseconds, selector_t*s)
{
	return new poller_impl_t(milliseconds, s);
}

poller_impl_t::poller_impl_t(int milliseconds, selector_t* s) :
	milliseconds(milliseconds),
	selector(s),
th(&poller_impl_t::run, this)
{

}
poller_impl_t::~poller_impl_t()
{
	shutdown = true;
	th.join();
}

void
poller_impl_t::run()
{
	while (!shutdown)
	{
		selector->poll(milliseconds);
	}
}

