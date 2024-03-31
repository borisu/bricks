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
		// Subtree accessors
		// 
		virtual bool
			remove_subtree(const xp_t& xp) override;

		//
		// Node accessors.
		//
		virtual optional<bricks_handle_t>
			get_root() const override;

		virtual optional<bricks_handle_t>
			get_node(const xp_t& xp) const override;

		virtual optional<bricks_handle_t>
			add_node(const xp_t& xp, bool replicate_leaf = false) override;

		//
		// Name Accessors
		//
		virtual optional<string>
			get_node_name(const xp_t& xp) const override;

		virtual bool
			set_node_name(const xp_t& xp, const char *name) override;

		//
		// Node value accessors.
		//
		virtual optional<const buffer_t*>
			get_node_value(const xp_t& xp) const override;

		virtual bool
			set_node_value(const xp_t& xp, const char*, int len) override;

		virtual bool
			remove_node_value(const xp_t& xp) override;

		//
		// Node property accessors.
		//
		virtual optional<property_value_t>
			get_property_value(const xp_t& xp, const char* property_name) const override;

		virtual bool
			set_property_value(const xp_t& xp, const char* property_name, property_value_t v) override;

		virtual bool
			remove_property(const xp_t& xp, const char* property_name) override;

		//
		// Children accessors.
		//
		virtual optional<size_t>
			get_node_children_count(const xp_t& xp) const override;

		//
		// Utils.
		//
		virtual bool
			traverse(xtree_visitor_t*) const override;

		virtual bool
			traverse(const xp_t& xp, xtree_visitor_t*) const override;

		virtual void release() override { delete this; };

		virtual ~xtree_impl_t();

	protected:

		struct xnode_t
		{
			string			name;
			property_list_t properties;
			buffer_t		value;
			list<xnode_t>	children;
			xnode_t*		parent	= nullptr; // NULL only for hidden root

			bool is_genesis() { return parent == nullptr; }

			bool is_root() { return parent != nullptr && parent->is_genesis(); }
		};

		virtual optional<xnode_t*>
			get_node_rec(const xp_t& xp) const;

		virtual optional<xnode_t*>
			get_node_rec(const xp_t& xp, bool create, bool replicate_leaf);

		// non const variant
		virtual optional<xnode_t*>
			get_node_rec1(xnode_t& node, const string_view& path, int index, bool create, bool replicate_leaf, bool new_node);

		virtual bool
			traverse(const xnode_t& node, xtree_visitor_t*) const;

	private:
	
		xnode_t genesis;

	};
}

	








