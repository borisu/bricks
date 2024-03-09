#include "pch.h"
#include "xtree_api.h"
#include "xtree.h"

xtree_t* create_xtree()
{
	return new xtree_impl_t();
}


