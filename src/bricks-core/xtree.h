#pragma once
#include "common.h"
#include <any>

using namespace std;

namespace bricks
{
	typedef pair<string, any> property_t;
	typedef list<property_t> properties_list_t;

	class xtree_visitor_t
	{
	public:
		virtual bool start_element(const string& name, const properties_list_t& properties, const buffer_t& value) = 0;

		virtual bool end_element(const string& name) = 0;
	};

	class BRICKS_API xtree_t : public brick_t
	{
	public:

		//
		// Subtree accessors
		// 

		virtual void
			remove_subtree(const string_view& path) = 0;

		virtual void 
			remove_subtree(bricks_handle_t node, const string_view& path) = 0;

		//
		// Node accessors.
		//
		virtual optional<bricks_handle_t> 
			get_root() const = 0;

		virtual optional<bricks_handle_t>
			get_node(const string_view& path) const = 0;

		virtual optional<bricks_handle_t>
			get_node(bricks_handle_t node, const string_view& path) const = 0;

		virtual optional<bricks_handle_t>
			add_node(const string_view& path, bool replicate_leaf = false) = 0;

		virtual optional<bricks_handle_t>
			add_node(bricks_handle_t node, const string_view& path, bool replicate_leaf = false) = 0;

		//
		// Node value accessors.
		//
		virtual optional<const buffer_t*>
			get_node_value(const string_view& path) const = 0;

		virtual optional<bricks_handle_t>
			set_node_value(const string_view& path, const char*, int len, bool create = true) = 0;

		virtual optional<bricks_handle_t>
			set_node_value(bricks_handle_t node, const string_view& path, const char*, int len, bool create = true) = 0;

		virtual void
			remove_node_value(const string_view& path) = 0;

		virtual void
			remove_node_value(bricks_handle_t node, const string_view& path)  = 0;

		//
		// Node property accessors.
		//
		virtual optional<string>
			get_property_value_as_string(const string_view& path, const string_view& property_name) const = 0;

		virtual optional<int>
			get_property_value_as_int(const string_view& path, const string_view& property_name) const = 0;

		virtual optional<bool>
			get_property_value_as_bool(const string_view& path, const string_view& property_name) const = 0;

		virtual optional<double>
			get_property_value_as_double(const string_view& path, const string_view& property_name) const = 0;

		virtual bool
			set_property_value(const string_view& path, const string_view& property_name, int value, bool create = true) = 0;

		virtual bool
			set_property_value(const string_view& path, const string_view& property_name, bool value, bool create = true) = 0;

		virtual bool
			set_property_value(const string_view& path, const string_view& property_name, double value, bool create = true) = 0;

		virtual bool
			set_property_value(const string_view& path, const string_view& property_name, const string_view& value, bool create = true) = 0;

		virtual bool
			set_property_value(bricks_handle_t node, const string_view& path, const string_view& property_name, int, bool create = true) = 0;

		virtual bool
			set_property_value(bricks_handle_t node, const string_view& path, const string_view& property_name, bool, bool create = true) = 0;

		virtual bool
			set_property_value(bricks_handle_t node, const string_view& path, const string_view& property_name, double, bool create = true) = 0;

		virtual bool
			set_property_value(bricks_handle_t node, const string_view& path, const string_view& property_name, const string_view&, bool create = true) = 0;

		virtual void 
			remove_property(const string_view& path, const string_view& property_name) = 0;

		virtual void 
			remove_property(bricks_handle_t node, const string_view& path, const string_view& property_name) = 0;
		
		//
		// Children accessors.
		//
		virtual optional<size_t>
			get_node_children_count(const string_view& path) const = 0;

		virtual optional<size_t>
			get_node_children_count(bricks_handle_t node, const string_view& path) const = 0;

		virtual optional<string>
			get_child_name_by_index(const string_view& path, int i) const = 0;

		virtual optional<string>
			get_child_name_by_index(bricks_handle_t node, const string_view& path, int i) const = 0;

		//
		// Children property accessors.
		//
		virtual optional<string>
			get_child_property_value_as_string(const string_view& path, int index, const string_view& property_name) const = 0;

		virtual optional<int>
			get_child_property_value_as_int(const string_view& path, int index, const string_view& property_name) const = 0;

		virtual optional<bool>
			get_child_property_value_as_bool(const string_view& path, int index, const string_view& property_name) const = 0;

		virtual optional<double>
			get_child_property_value_as_double(const string_view& path, int index, const string_view& property_name) const = 0;

		//
		// Utils.
		//
		virtual bool 
			traverse(xtree_visitor_t*) const = 0;

		virtual bool
			traverse(const string_view& path, xtree_visitor_t*) const = 0;

		virtual bool
			traverse(bricks_handle_t node, const string_view& path, xtree_visitor_t*) const = 0;

		virtual ~xtree_t() {};

	};

	BRICKS_API xtree_t* create_xtree();

	BRICKS_API xtree_t* create_xtree_from_xml(const char* xml);

	BRICKS_API string   serialize_xtree_to_xml(const xtree_t*);

	BRICKS_API bool clone_xtree(xtree_t* src, optional<bricks_handle_t> src_h, const string_view &path, xtree_t* dst, optional<bricks_handle_t> dst_h);
	
}