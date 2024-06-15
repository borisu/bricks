#pragma once
#include <variant>
#include "common.h"
#include "buffer.h"


using namespace std;

namespace bricks
{
	typedef variant<int, long long, double, bool, string> property_value_t;
	typedef pair<string, property_value_t> property_t;
	typedef list<property_t> property_list_t;

	struct xp_t
	{
		xp_t() {};

		xp_t(bricks_handle_t a):anchor(a) {}

		xp_t(bricks_handle_t a, const char *p) :anchor(a),path(p) {}

		xp_t(bricks_handle_t a, const char* p, int i) :anchor(a), path(p), index(i) {}

		xp_t(const char* p) :path(p){}

		xp_t(const char* p, int i) :path(p), index(i) {}

		xp_t& set_rel_path(const char* p) { path = p; return *this; }
		
		optional<bricks_handle_t> anchor;

		optional<string> path;

		optional<int> index;

	};
	
	class xtree_visitor_t
	{
	public:
		virtual bool start_element(const string& name, const property_list_t& properties, const vector_t& value) = 0;

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
			set_node_name(const xp_t& xp, const char *name) = 0;

		//
		// Node value accessors.
		//
		virtual optional<const vector_t*>
			get_node_value(const xp_t& xp) const = 0;

		virtual bool
			set_node_value(const xp_t& xp, const char* buf, int len) = 0;

		virtual bool
			remove_node_value(const xp_t& xp) = 0;

		//
		// Node property accessors.
		//
		virtual optional<property_value_t>
			get_property_value(const xp_t& xp, const char* property_name) const = 0;

		virtual optional<string>
			get_property_value_as_string(const xp_t& xp, const char* property_name) const = 0;

		virtual bool
			set_property_value(const xp_t& xp, const char* property_name, property_value_t v) = 0;

		virtual bool 
			remove_property(const xp_t& xp,  const char* property_name) = 0;
		
		//
		// Children accessors.
		//
		virtual optional<size_t>
			get_node_children_count(const xp_t& xp) const = 0;

		//
		// Utils.
		//
		virtual bool 
			traverse(xtree_visitor_t* visitor) const = 0;

		virtual bool
			traverse(const xp_t& xp, xtree_visitor_t*visitor) const = 0;

		virtual ~xtree_t() {};

	};

	BRICKS_API xtree_t* create_xtree();

	BRICKS_API xtree_t* create_xtree_from_xml(const char* xml);

	BRICKS_API string serialize_xtree_to_xml(const xtree_t*);

	BRICKS_API bool clone_xtree(const xtree_t* src, const xp_t& xp_src, xtree_t* dst, const xp_t& xp_dst);

	template <typename T1, typename T2>
	auto get_opt(std::optional<T2> opt, T1 d = T1{})
	{
		return opt.has_value() ? opt.value() : d;
	}

	template <typename T1>
	T1 get_opt(std::optional<property_value_t> opt, T1 d = T1{})
	{
		try
		{
			return opt.has_value() ? std::get<T1>(opt.value()) : d;
		}
		catch (std::bad_variant_access const&)
		{
			return d;
		}
	}
	
}