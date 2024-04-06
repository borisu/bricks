#pragma once
#include "bricks.h"

namespace bricks
{

	class buffer_impl_t : public buffer_t
	{
	public:

		buffer_impl_t(const char* data, int size);

		virtual int size() override;

		virtual const char* data() override;

		virtual void release() { delete this; }

	protected:

		vector<char> vec;
	};

}