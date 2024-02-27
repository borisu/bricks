#pragma once
#include "bricks.h"

using namespace tinyxml2;

class xtree_t
{
public:

	virtual bricks_error_code_e 
	load(const char* file) = 0;

	virtual bricks_error_code_e
	parse(const char* string) = 0;
	
	virtual int
	get_node_children_count(const char* c) = 0;

	virtual const char*
	get_node_property(const char* c, const char* property) = 0;

	virtual int
	get_node_property_as_int(const char* c, const char* property) = 0;

	virtual bool
	get_node_property_as_bool(const char* c, const char* property) = 0;

	virtual const char*
	get_node_property_by_index(const char* c, int index, const char* property) = 0;

private:
	

};








