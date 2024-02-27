#pragma once
#include "xtree.h"

class xml_xtree_t : public xtree_t
{
public:

	virtual bricks_error_code_e
	load(const char* file) override;

	virtual bricks_error_code_e
	parse(const char* string) override;

	virtual int
	get_node_children_count(const char* c) override;

	virtual const char*
	get_node_property(const char* c, const char* property) override;

	virtual int
	get_node_property_as_int(const char* c, const char* property) override;

	virtual bool
	get_node_property_as_bool(const char* c, const char* property) override;

	virtual const char*
	get_node_property_by_index(const char* c, int index, const char* property) override;

private:

	XMLElement* walk_path(XMLElement* h, const char* c);

	XMLDocument doc;
};

