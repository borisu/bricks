#pragma once
#include "bricks_api.h"

class xtree_t
{
public:

	virtual bricks_error_code_e
	load(const char* file) = 0;

	virtual bricks_error_code_e
	parse(const char* string) = 0;

	virtual const char*
	get_node_value(const char* np) = 0;

	virtual int 
	get_node_children_count(const char* np) = 0;

	virtual const char*
	get_child_value_by_index(const char* np, int i) = 0;

	virtual const char*
	get_node_property(const char* np, const char* pp) = 0;

	virtual const char*
	get_child_property_by_index(const char* np, int i, const char* pp) = 0;

	virtual int
	get_node_property_as_int(const char* np, const char* pp) = 0;

	virtual int
	get_child_property_as_int_by_index(const char* np, int i, const char* pp) = 0;

	virtual bool
	get_node_property_as_bool(const char* np, const char* pp) = 0;

	virtual bool
	get_child_property_as_bool_by_index(const char* np, int i, const char* pp) = 0;

	virtual double
	get_node_property_as_double(const char* np, const char* pp) = 0;

	virtual double
	get_child_property_as_double_by_index(const char* np, int i, const char* pp) = 0;

	virtual ~xtree_t() {};

private:
	

};








