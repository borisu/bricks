#pragma once
#include "bricks.h"

using namespace std;

namespace bricks {


	class xtree_impl_t :
		public xtree_t
	{
	public:

		xtree_impl_t();

		//
		// Tree acceessors
		//
		virtual void
			remove_subtree(const string_view& path) override;

		virtual void
			remove_subtree(bricks_handle_t node, const string_view& path) override;

		//
		// Node accessors.
		//
		virtual optional<bricks_handle_t>
			get_node(const string_view& path) const override;

		virtual optional<bricks_handle_t>
			get_node(bricks_handle_t node, const string_view& path) const;

		virtual optional<bricks_handle_t>
			add_node(const string_view& path, bool replicate_leaf) override;

		virtual optional<bricks_handle_t>
			add_node(bricks_handle_t node, const string_view& path, bool replicate_leaf);

		//
		// Node value accessors.
		//
		virtual optional<const buffer_t*>
			get_node_value(const string_view& path) const override;

		virtual optional<bricks_handle_t>
			set_node_value(const string_view& path, const char*, int len, bool create) override;

		virtual optional<bricks_handle_t>
			set_node_value(bricks_handle_t node, const string_view& path, const char*, int len, bool create) override;

		virtual void
			remove_node_value(const string_view& path) override;

		virtual void
			remove_node_value(bricks_handle_t node, const string_view& path) override;

		//
		// Node property accessors.
		//
		virtual optional<string>
			get_property_value_as_string(const string_view& path, const string_view& property_name) const override;

		virtual optional<int>
			get_property_value_as_int(const string_view& path, const string_view& property_name) const override;

		virtual optional<bool>
			get_property_value_as_bool(const string_view& path, const string_view& property_name) const override;

		virtual optional<double>
			get_property_value_as_double(const string_view& path, const string_view& property_name) const override;

		virtual bool
			set_property_value(const string_view& path, const string_view& property_name, int, bool create) override;

		virtual bool
			set_property_value(const string_view& path, const string_view& property_name, bool, bool create) override;

		virtual bool
			set_property_value(const string_view& path, const string_view& property_name, double, bool create) override;

		virtual bool
			set_property_value(const string_view& path, const string_view& property_name, const string_view&, bool create) override;

		virtual bool
			set_property_value(bricks_handle_t node, const string_view& path, const string_view& property_name, int value, bool create) override;

		virtual bool
			set_property_value(bricks_handle_t node, const string_view& path, const string_view& property_name, bool value, bool create) override;

		virtual bool
			set_property_value(bricks_handle_t node, const string_view& path, const string_view& property_name, double value, bool create) override;

		virtual bool
			set_property_value(bricks_handle_t node, const string_view& path, const string_view& property_name, const string_view& value, bool create) override;

		virtual void
			remove_property(const string_view& path, const string_view& property_name) override;

		virtual void
			remove_property(bricks_handle_t node, const string_view& path, const string_view& property_name) override;
		
		//
		// Children accessors.
		//
		virtual optional<size_t>
			get_node_children_count(const string_view& path) const override;

		virtual optional<size_t>
			get_node_children_count(bricks_handle_t node, const string_view& path) const override;

		virtual optional<string>
			get_child_name_by_index(const string_view& path, int index) const override;

		virtual optional<string>
			get_child_name_by_index(bricks_handle_t node, const string_view& path, int i) const override;

		//
		// Children property accessors.
		//
		virtual optional<string>
			get_child_property_value_as_string(const string_view& path, int index, const string_view& property_name) const override;

		virtual optional<int>
			get_child_property_value_as_int(const string_view& path, int index, const string_view& property_name) const override;

		virtual optional<bool>
			get_child_property_value_as_bool(const string_view& path, int index, const string_view& property_name) const override;

		virtual optional<double>
			get_child_property_value_as_double(const string_view& path, int index, const string_view& property_name) const override;

		//
		// Utils.
		//
		virtual void traverse(xtree_visitor_t*) const override;

		virtual void traverse(const string_view& path, xtree_visitor_t*) const override;

		virtual void traverse(bricks_handle_t node, const string_view& path, xtree_visitor_t*) const override;

		virtual void release() override { delete this; };

		virtual ~xtree_impl_t();

	protected:

		struct xnode_t
		{
			string name;
			list<pair<string, any>> properties;
			buffer_t value;
			list<xnode_t> children;
			xnode_t* parent = nullptr;
		};

		virtual optional<xnode_t*>
			get_node_rec(const xnode_t& node, const string_view& path) const;

		// non const variant
		virtual optional<xnode_t*>
			get_node_rec1(xnode_t& node, const string_view& path, bool create, bool replicate_leaf = false);

		virtual optional<xnode_t*>
			get_child(const string_view& path, int index) const;

		virtual optional<xnode_t*>
			get_child(bricks_handle_t h, const string_view& path, int index) const;

		virtual optional<xnode_t*>
			get_child(optional<xnode_t*> node, int index) const;

		virtual optional<bricks_handle_t> 
			set_node_value(optional<xtree_impl_t::xnode_t*>, const char* buf, int len);

		virtual bool 
			set_property_value(optional<xtree_impl_t::xnode_t*>, const string_view& property_name, std::any);

		virtual void
			remove_property(optional<xtree_impl_t::xnode_t*>, const string_view& property_name);

		virtual void
			remove_value(optional<xtree_impl_t::xnode_t*>);

		virtual void
			remove_subtree(optional<xtree_impl_t::xnode_t*>);

		virtual void
			traverse(optional<xtree_impl_t::xnode_t*>, xtree_visitor_t*) const;

		virtual void 
			traverse(xtree_visitor_t*, const xnode_t&) const;


	private:

		template<typename T>
		optional<T>	get_property_value_as1(optional<xtree_impl_t::xnode_t*> node, const string_view& property_name) const;

		template<typename T>
		optional<T>	get_property_value_as(const string_view& path, const string_view& property_name) const;

		template<typename T>
		optional<T>	get_child_property_value_as(const string_view& path, int indexndex, const string_view& property_name) const;

		xnode_t root;

	};
}








