#pragma once
#include "bricks_api.h"
#include "xtree_api.h"
#include "tinyxml2.h"


using namespace std;

class xtree_impl_t: 
	public xtree_t
{
public:
	xtree_impl_t();

	virtual bricks_error_code_e 
	load_from_xml(const char* str) override;

	virtual optional<const buffer_t *>
	get_node_value(const string& path) override;

	// opaque node handle
	virtual optional<bricks_handle_t>
	get_node(const string& path) override;

	virtual optional<string>
	get_property_value_as_string(const string& path, const string &property_name) override;

	virtual optional<int>
	get_property_value_as_int(const string& path, const string &property_name) override;

	virtual optional<bool>
	get_property_value_as_bool(const string& path, const string &property_name) override;

	virtual optional<double>
	get_property_value_as_double(const string& path, const string &property_name) override;

	virtual optional<int>
	get_node_children_count(const string& path) override;

	virtual optional<int>
	get_node_children_count(bricks_handle_t node, const string& path) override;

	virtual optional<string>
	get_child_name_by_index(const string& path, int index) override;

	virtual optional<string>
	get_child_property_value_as_string(const string& path, int index, const string &property_name) override;

	virtual optional<int>
	get_child_property_value_as_int(const string& path, int index, const string &property_name) override;

	virtual optional<bool>
	get_child_property_value_as_bool(const string& path, int index, const string &property_name) override;

	virtual optional<double>
	get_child_property_value_as_double(const string& path, int index, const string &property_name) override;

	virtual ~xtree_impl_t();

protected:

	struct xnode_t
	{
		string name;
		map<string, any> properties;
		buffer_t value;
		list<xnode_t> children;
	};
	
	virtual optional<xnode_t*>
	get_node(xnode_t& node, string_view&& path, bool create);

	virtual optional<xnode_t*>
	get_child(const string& path, int indexndex);

private:

	template<typename T>
	optional<T>	get_property_value_as(const string& path, const string& property_name);

	template<typename T>
	optional<T>	get_child_property_value_as(const string& path, int indexndex, const string& property_name);

	bricks_error_code_e
	traverse_elements(const tinyxml2::XMLElement* element, xnode_t& parent_node);

	tinyxml2::XMLDocument doc;

	xnode_t root;

};








