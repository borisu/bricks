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
		// Node accessors.
		//
		virtual optional<bricks_handle_t>
			get_node(const string& path) const override;

		virtual optional<bricks_handle_t>
			add_node(const string& path) override;

		virtual optional<bricks_handle_t>
			get_node(bricks_handle_t node, const string& path) const;

		virtual optional<bricks_handle_t>
			add_node(bricks_handle_t node, const string& path);

		//
		// Node value accessors.
		//
		virtual optional<const buffer_t*>
			get_node_value(const string& path) const override;

		virtual optional<bricks_handle_t>
			set_node_value(const string& path, const char*, int len, bool create) override;

		virtual optional<bricks_handle_t>
			set_node_value(bricks_handle_t node, const string& path, const char*, int len, bool create) override;

		//
		// Node property accessors.
		//
		virtual optional<string>
			get_property_value_as_string(const string& path, const string& property_name) const override;

		virtual optional<int>
			get_property_value_as_int(const string& path, const string& property_name) const override;

		virtual optional<bool>
			get_property_value_as_bool(const string& path, const string& property_name) const override;

		virtual optional<double>
			get_property_value_as_double(const string& path, const string& property_name) const override;

		virtual bool
			set_property_value(const string& path, const string& property_name, int, bool create) override;

		virtual bool
			set_property_value(const string& path, const string& property_name, bool, bool create) override;

		virtual bool
			set_property_value(const string& path, const string& property_name, double, bool create) override;

		virtual bool
			set_property_value(const string& path, const string& property_name, const string&, bool create) override;

		virtual bool
			set_property_value(bricks_handle_t node, const string& path, const string& property_name, int value, bool create) override;

		virtual bool
			set_property_value(bricks_handle_t node, const string& path, const string& property_name, bool value, bool create) override;

		virtual bool
			set_property_value(bricks_handle_t node, const string& path, const string& property_name, double value, bool create) override;

		virtual bool
			set_property_value(bricks_handle_t node, const string& path, const string& property_name, const string& value, bool create) override;
		

		//
		// Children accessors.
		//
		virtual optional<size_t>
			get_node_children_count(const string& path) const override;

		virtual optional<size_t>
			get_node_children_count(bricks_handle_t node, const string& path) const override;

		virtual optional<string>
			get_child_name_by_index(const string& path, int index) const override;

		virtual optional<string>
			get_child_name_by_index(bricks_handle_t node, const string& path, int i) const override;

		//
		// Children property accessors.
		//
		virtual optional<string>
			get_child_property_value_as_string(const string& path, int index, const string& property_name) const override;

		virtual optional<int>
			get_child_property_value_as_int(const string& path, int index, const string& property_name) const override;

		virtual optional<bool>
			get_child_property_value_as_bool(const string& path, int index, const string& property_name) const override;

		virtual optional<double>
			get_child_property_value_as_double(const string& path, int index, const string& property_name) const override;

		//
		// Utils.
		//
		virtual void traverse(xtree_visitor_t*) const override;

		virtual void release() override { delete this; };

		virtual ~xtree_impl_t();

	protected:

		struct xnode_t
		{
			string           name;
			map<string, any> properties;
			buffer_t         value;
			list<xnode_t>    children;
		};

		virtual optional<xnode_t*>
			get_node_rec(const xnode_t& node, const string_view& path) const;

		// non const variant
		virtual optional<xnode_t*>
			get_node_rec1(xnode_t& node, const string_view& path, bool create);

		virtual optional<xnode_t*>
			get_child(const string& path, int index) const;

		virtual optional<xnode_t*>
			get_child(bricks_handle_t h, const string& path, int index) const;

		virtual optional<xnode_t*>
			get_child(optional<xnode_t*> node, int index) const;

		virtual optional<bricks_handle_t> 
			set_node_value(optional<xtree_impl_t::xnode_t*>, const char* buf, int len);

		virtual bool set_property_value(optional<xtree_impl_t::xnode_t*>, const string& property_name, std::any);

	private:

		template<typename T>
		optional<T>	get_property_value_as(const string& path, const string& property_name) const;

		template<typename T>
		optional<T>	get_child_property_value_as(const string& path, int indexndex, const string& property_name) const;

		void traverse(xtree_visitor_t*, const xnode_t &) const;

		xnode_t root;

	};
}








