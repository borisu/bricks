#pragma once
#include "bricks_api.h"
#include <string>
#include <optional>

using namespace std;

class BRICKS_API xtree_t
{
public:

	virtual bricks_error_code_e 
	load_from_xml(const char* str) = 0;

	// opaque node handle
	virtual optional<bricks_handle_t>
	get_node(const string& path) const = 0;

	virtual optional<const buffer_t*>
	get_node_value(const string& path) const = 0;

	virtual optional<bricks_handle_t>
	set_node_value(const string& path, const buffer_t*, bool create = true) = 0;

	virtual optional<bricks_handle_t>
	set_node_value(const string& path, const char*, size_t len, bool create = true) = 0;
	
	virtual optional<size_t>
	get_node_children_count(const string& path) const = 0;

	virtual optional<size_t>
	get_node_children_count(bricks_handle_t node, const string& path) const = 0;

	virtual optional<string>
	get_child_name_by_index(const string& path, int i) const = 0;

	virtual optional<string>
	get_property_value_as_string(const string& path, const string& property_name) const = 0;

	virtual optional<int>
	get_property_value_as_int(const string& path, const string& property_name) const = 0;

	virtual optional<bool>
	get_property_value_as_bool(const string& path, const string& property_name) const = 0;

	virtual optional<double>
	get_property_value_as_double(const string& path, const string& property_name) const = 0;

	virtual optional<string>
	get_child_property_value_as_string(const string& path, int index, const string& property_name) const = 0;

	virtual optional<int>
	get_child_property_value_as_int(const string& path, int index, const string& property_name) const = 0;

	virtual optional<bool>
	get_child_property_value_as_bool(const string& path, int index, const string& property_name) const = 0;

	virtual optional<double>
	get_child_property_value_as_double(const string& path, int index, const string& property_name) const = 0;

	virtual ~xtree_t() {};

};

BRICKS_API xtree_t* create_xtree();

BRICKS_API void destroy_xtree(xtree_t* tree);