#pragma once
#include "common.h"
#include <any>

using namespace std;

namespace bricks
{
	class xtree_visitor_t
	{
	public:
		virtual void start_element(const string& name, const map<string, any>& properties, const buffer_t& value) = 0;

		virtual void end_element(const string& name) = 0;
	};

	class BRICKS_API xtree_t : public brick_t
	{
	public:

		//
		// Node accessors.
		//
		virtual optional<bricks_handle_t>
			get_node(const string& path) const = 0;

		virtual optional<bricks_handle_t>
			add_node(const string& path) = 0;

		virtual optional<bricks_handle_t>
			get_node(bricks_handle_t node, const string& path) const = 0;

		virtual optional<bricks_handle_t>
			add_node(bricks_handle_t node, const string& path) = 0;

		//
		// Node value accessors.
		//
		virtual optional<const buffer_t*>
			get_node_value(const string& path) const = 0;

		virtual optional<bricks_handle_t>
			set_node_value(const string& path, const char*, int len, bool create = true) = 0;

		virtual optional<bricks_handle_t>
			set_node_value(bricks_handle_t node, const string& path, const char*, int len, bool create = true) = 0;

		//
		// Node property accessors.
		//
		virtual optional<string>
			get_property_value_as_string(const string& path, const string& property_name) const = 0;

		virtual optional<int>
			get_property_value_as_int(const string& path, const string& property_name) const = 0;

		virtual optional<bool>
			get_property_value_as_bool(const string& path, const string& property_name) const = 0;

		virtual optional<double>
			get_property_value_as_double(const string& path, const string& property_name) const = 0;

		virtual bool
			set_property_value(const string& path, const string& property_name, int value, bool create = true) = 0;

		virtual bool
			set_property_value(const string& path, const string& property_name, bool value, bool create = true) = 0;

		virtual bool
			set_property_value(const string& path, const string& property_name, double value, bool create = true) = 0;

		virtual bool
			set_property_value(const string& path, const string& property_name, const string& value, bool create = true) = 0;

		virtual bool
			set_property_value(bricks_handle_t node, const string& path, const string& property_name, int, bool create = true) = 0;

		virtual bool
			set_property_value(bricks_handle_t node, const string& path, const string& property_name, bool, bool create = true) = 0;

		virtual bool
			set_property_value(bricks_handle_t node, const string& path, const string& property_name, double, bool create = true) = 0;

		virtual bool
			set_property_value(bricks_handle_t node, const string& path, const string& property_name, const string&, bool create = true) = 0;

		//
		// Children accessors.
		//
		virtual optional<size_t>
			get_node_children_count(const string& path) const = 0;

		virtual optional<size_t>
			get_node_children_count(bricks_handle_t node, const string& path) const = 0;

		virtual optional<string>
			get_child_name_by_index(const string& path, int i) const = 0;

		virtual optional<string>
			get_child_name_by_index(bricks_handle_t node, const string& path, int i) const = 0;

		//
		// Children property accessors.
		//
		virtual optional<string>
			get_child_property_value_as_string(const string& path, int index, const string& property_name) const = 0;

		virtual optional<int>
			get_child_property_value_as_int(const string& path, int index, const string& property_name) const = 0;

		virtual optional<bool>
			get_child_property_value_as_bool(const string& path, int index, const string& property_name) const = 0;

		virtual optional<double>
			get_child_property_value_as_double(const string& path, int index, const string& property_name) const = 0;

		//
		// Utils.
		//
		virtual void traverse(xtree_visitor_t*) const = 0;

		virtual ~xtree_t() {};

	};

	BRICKS_API xtree_t* create_xtree();

	BRICKS_API xtree_t* create_xtree_from_xml(const char* xml);

	BRICKS_API string   serialize_xtree_to_xml(const xtree_t*);
	
}