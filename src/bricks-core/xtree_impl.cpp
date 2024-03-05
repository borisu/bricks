#include "pch.h"
#include "xtree_api.h"
#include "xml_xtree.h"


bricks_handle_t
bricks_create_xtree_from_xml_file(const char* f)
{
	xtree_t* xt = new xml_xtree_t();
	if ((xt->load(f)) != BRICKS_SUCCESS)
		delete xt;

	return (bricks_handle_t)xt;

}


bricks_handle_t
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

int
bricks_get_children_count(bricks_handle_t h, const char *np)
{
	return ((xtree_t*)h)->get_node_children_count(np);
}

void
bricks_destroy_xtree(bricks_handle_t h)
{
	delete ((xtree_t*)h);
}

 const char*
bricks_get_node_property(bricks_handle_t h, const char *np, const char *pp)
{
	return ((xtree_t*)h)->get_node_property(np, pp);
}

 const char*
bricks_get_node_value(bricks_handle_t h, const char* np)
{
	return ((xtree_t*)h)->get_node_value(np);
}

const char*
bricks_get_child_name_by_index(bricks_handle_t h, const char* np, int i)
{
	return ((xtree_t*)h)->get_child_name_by_index(np, i);
}

const char*
bricks_get_child_property_by_index(bricks_handle_t h, const char* np, int i, const char* pp)
{
	return ((xtree_t*)h)->get_child_property_by_index(np, i, pp);
}

int
bricks_get_node_property_as_int(bricks_handle_t h, const char* np, const char* pp)
{
	return ((xtree_t*)h)->get_node_property_as_int(np, pp);
}

int
bricks_get_child_property_as_int_by_index(bricks_handle_t h, const char* np, int i, const char* pp)
{
	return ((xtree_t*)h)->get_child_property_as_int_by_index(np, i, pp);
}

bool
bricks_get_node_property_as_bool(bricks_handle_t h, const char* np, const char* pp)
{
	return ((xtree_t*)h)->get_node_property_as_bool(np, pp);
}

bool
bricks_get_child_property_as_bool_by_index(bricks_handle_t h, const char* np, int i, const char* pp)
{
	return ((xtree_t*)h)->get_child_property_as_bool_by_index(np, i, pp);
}

double
bricks_get_node_property_as_double(bricks_handle_t h, const char* np, const char* pp)
{
	return ((xtree_t*)h)->get_node_property_as_double(np, pp);
}

double
bricks_get_child_property_as_double_by_index(bricks_handle_t h, const char* np, int i, const char* pp)
{
	return ((xtree_t*)h)->get_child_property_as_double_by_index(np, i, pp);
}

