#include "pch.h"
#include "xtree_api.h"
#include "xml_xtree.h"


BRICKS_API bricks_handle_t
bricks_create_xtree_from_xml_file(const char* f)
{
	xtree_t* xt = new xml_xtree_t();
	if ((xt->load(f)) != BRICKS_SUCCESS)
		delete xt;

	return (bricks_handle_t)xt;

}


BRICKS_API bricks_handle_t
bricks_create_xtree_from_xml_string(const char* f)
{
	xtree_t* xt = new xml_xtree_t();
	if ((xt->parse(f)) != BRICKS_SUCCESS)
	{
		delete xt;
		return (bricks_handle_t)nullptr;
	}

	return (bricks_handle_t)xt;

}

BRICKS_API int
bricks_get_node_children_count(bricks_handle_t h, const char* c)
{
	return ((xtree_t*)h)->get_node_children_count(c);
}
