#pragma once
#include "common.h"
#include <variant>

using namespace std;

namespace bricks
{
	typedef variant<int, double, bool, string> property_value_t;
	typedef pair<string, property_value_t> property_t;
	typedef list<property_t> property_list_t;

	struct xp_t
	{
		xp_t() {};

		xp_t(bricks_handle_t a, optional<int> index = {}) :anchor(a), index(index) {}

		xp_t(string_view v, optional<int> index = {}) :path(v), index(index) {}

		xp_t(const string &v, optional<int> index = {}) :path(v), index(index) {}

		xp_t(const char *v, optional<int> index = {}) :path(v), index(index) {}

		xp_t(bricks_handle_t a, string_view v, optional<int> index = {}) :anchor(a), path(v), index(index) {}

		xp_t(bricks_handle_t a, const string& v, optional<int> index = {}) :anchor(a), path(v), index(index) {}

		xp_t(bricks_handle_t a, const char *v, optional<int> index = {}) :anchor(a), path(v), index(index) {}

		const optional<bricks_handle_t> anchor;

		const optional<string_view> path;

		const optional<int> index;

	};
	
	class xtree_visitor_t
	{
	public:
		virtual bool start_element(const string& name, const property_list_t& properties, const buffer_t& value) = 0;

		virtual bool end_element(const string& name) = 0;
	};

	class BRICKS_API xtree_t : public brick_t
	{
	public:
		//
		// Subtree accessors
		// 
		virtual bool
			remove_subtree(const xp_t& xp) = 0;
		
		//
		// Node accessors.
		//
		virtual optional<bricks_handle_t> 
			get_root() const = 0;

		virtual optional<bricks_handle_t>
			get_node(const xp_t& xp) const = 0;

		virtual optional<bricks_handle_t>
			add_node(const xp_t& xp, bool replicate_leaf = false) = 0;

		//
		// Name Accessors
		//
		virtual optional<string>
			get_node_name(const xp_t& xp) const = 0;

		virtual bool
			set_node_name(const xp_t& xp, const string_view& name) const = 0;

		//
		// Node value accessors.
		//
		virtual optional<const buffer_t*>
			get_node_value(const xp_t& xp) const = 0;

		virtual bool
			set_node_value(const xp_t& xp, const char*, int len) = 0;

		virtual bool
			remove_node_value(const xp_t& xp) = 0;

		//
		// Node property accessors.
		//
		virtual optional<property_value_t>
			get_property_value(const xp_t& xp, const string_view& property_name) const = 0;

		virtual bool
			set_property_value(const xp_t& xp, const string_view& property_name, property_value_t v) = 0;

		virtual bool 
			remove_property(const xp_t& xp,  const string_view& property_name) = 0;
		
		//
		// Children accessors.
		//
		virtual optional<size_t>
			get_node_children_count(const xp_t& xp) const = 0;

		//
		// Utils.
		//
		virtual bool 
			traverse(xtree_visitor_t*) const = 0;

		virtual bool
			traverse(const xp_t& xp, xtree_visitor_t*) const = 0;

		virtual ~xtree_t() {};

	};

	BRICKS_API xtree_t* create_xtree();

	BRICKS_API xtree_t* create_xtree_from_xml(const char* xml);

	BRICKS_API string serialize_xtree_to_xml(const xtree_t*);

	BRICKS_API bool clone_xtree(const xtree_t* src, const xp_t& xp_src, xtree_t* dst, const xp_t& xp_dst);
	
}