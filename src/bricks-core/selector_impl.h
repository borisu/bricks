#pragma once
#include "selector.h"

namespace bricks
{
	class selector_impl_t : 
		public selector_t
	{
		virtual bricks_error_code_e init(cb_queue_t *queue) override;

		virtual bricks_error_code_e poll(int milliseconds) override;

		virtual cb_queue_t* queue() override;

		virtual void release() override { delete this; };

	protected:

		cb_queue_t* q;
	};

}


