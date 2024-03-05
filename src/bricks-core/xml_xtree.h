#pragma once
#include "xtree.h"

using namespace tinyxml2;

class xml_xtree_t : public xtree_t
{
public:

	virtual bricks_error_code_e
	load(const char* file) override;

	virtual bricks_error_code_e
	parse(const char* string) override;

	virtual const char*
	get_node_value(const char* np) override;

	virtual const char*
	get_child_name_by_index(const char* np, int i) override;

	virtual int
	get_node_children_count(const char* np) override;

	virtual const char*
	get_node_property(const char* np, const char* pp) override;

	virtual const char*
	get_child_property_by_index(const char* np, int i, const char* pp) override;

	virtual int
	get_node_property_as_int(const char* np, const char* pp) override;

	virtual int
	get_child_property_as_int_by_index(const char* np, int i, const char* pp) override;

	virtual bool
	get_node_property_as_bool(const char* np, const char* pp) override;

	virtual bool
	get_child_property_as_bool_by_index(const char* np, int i, const char* pp) override;

	virtual double
	get_node_property_as_double(const char* np, const char* pp) override;

	virtual double
	get_child_property_as_double_by_index(const char* np, int i, const char* pp) override;

	virtual ~xml_xtree_t() ;

private:

	XMLElement* walk_path(XMLElement* h, const char* c);

	XMLDocument doc;
};

