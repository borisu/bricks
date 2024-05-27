#pragma once
#include "selector.h"
#include "timer.h"
#include "utils.h"

namespace bricks
{
	class poller_impl_t : public poller_t
	{
	public:

		poller_impl_t(int milliseconds, selector_t* s);

		~poller_impl_t();

		virtual void release() override { delete this; };

	protected:

		void run();

		selector_t* selector = nullptr;

		atomic<bool> shutdown = false;

		int milliseconds = BRICKS_DEFAULT_CLIENT_TIMEOUT;

		std::thread th;
	};

	
}

