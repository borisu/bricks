#include "pch.h"
#include "xtree_impl.h"

using namespace bricks;

#define CONVERT_TO_OPAQUE

xtree_t* 
bricks::create_xtree()
{
	return new xtree_impl_t();
}

xtree_impl_t::xtree_impl_t() 
{
	genesis.name = "/";
};

optional<bricks_handle_t>
xtree_impl_t::get_root() const
{
	if (genesis.children.size() == 0)
		return {};

	return  optional<bricks_handle_t> ((bricks_handle_t) &genesis.children.back());
}

optional<xtree_impl_t::xnode_t*>
xtree_impl_t::get_node_rec(const xp_t& xp, bool create = false, bool replicate_leaf = false) 
{
	
	optional<xtree_impl_t::xnode_t*> ret = 
		get_node_rec1(
			xp.anchor.has_value() ? *(xnode_t*)xp.anchor.value() : genesis,
			xp.path.has_value()   ? xp.path.value()   : string_view(""),
			xp.index.has_value()  ? xp.index.value()  : -1,
			create,
			replicate_leaf,
			false);

	// do not return hidden root as a result
	return  (ret.has_value() && ret.value()->parent == nullptr) ? optional<xtree_impl_t::xnode_t*>{} : ret;

}

optional<xtree_impl_t::xnode_t*>
xtree_impl_t::get_node_rec1(xnode_t& node, const string_view& path, int index, bool create, bool replicate_leaf, bool new_node)
{
	bool new_child  = false;

	// non target logic inside while
	do
	{
		auto s = path.size();
		auto l = path.length();


		auto start = path.find_first_not_of('/'); // no more valid node name characters
		if (start == string_view::npos)
			break;

		auto end = path.find_first_of('/', start);
		if (end == string_view::npos)
			end = path.length();

		// find next node to descend
		string_view child_name(&path[start], end - start);

		auto it = find_if(node.children.begin(), node.children.end(),
			[&](const xnode_t& child) { return child.name == child_name; });

		if (it == node.children.end())
		{
			if (create)
			{
				// cannot create another root
				if (node.is_genesis() && node.children.size() > 0)
					return {};

				node.children.push_back(xnode_t{});

				node.children.back().name = child_name;
				node.children.back().parent = &node;

				it = --node.children.end();

				new_child = true;
			}
			else
			{
				return {};
			}
		}

		//MSVC does not create string_view with zero length path so we need special handling for that
		return get_node_rec1(*it, end == path.length() ? "" : string_view(&path[end], path.length() - end), index, create, replicate_leaf, new_child);

	} while (false);

	// OK I am the target node of the path

	// never return genesis root as a target node
	if (node.is_genesis())
		return {};

	// take care of replication
	if (create && replicate_leaf && !new_node)
	{
		// never replicate root node
		if (node.is_root())
			return {};
	
		node.parent->children.push_back(xnode_t{});
		node.parent->children.back().name = node.name;
		node.parent->children.back().parent = node.parent;
		
		return &node.parent->children.back();
	}

	// take care of index query
	if (index >= 0)
	{
		// actually index was requested
		if (index >= node.children.size())
			return {};

		auto it = node.children.begin();
		std::advance(it, index);

		return &(*it);
	}

	return &node;

}

optional<xtree_impl_t::xnode_t*>
xtree_impl_t::get_node_rec(const xp_t& xp) const
{
	return  const_cast<xtree_impl_t*>(this)->get_node_rec(xp, false,false);
}

optional<string>
xtree_impl_t::get_node_name(const xp_t& xp) const
{
	auto node = get_node_rec(xp);

	return node.has_value() ? optional<string>{ node.value()->name } : optional<string>{};
}

bool
xtree_impl_t::set_node_name(const xp_t& xp, const char* name)
{
	auto node = get_node_rec(xp,true,false);

	return node.has_value() ? 
		node.value()->name = name, 
		true : false;

}

bool
xtree_impl_t::set_node_value(const xp_t& xp, const char* buf, int len)
{
	auto node = get_node_rec(xp, true, false);

	return  node.has_value() ? 
		node.value()->value.resize(len),
		node.value()->value.assign(buf, buf + len), 
		true : false;
	
}

optional<const vector_t*>
xtree_impl_t::get_node_value(const xp_t& xp) const
{
	auto node = get_node_rec(xp);

	return node.has_value() ? &(node.value()->value) : optional<const vector_t*>{};
}

bool
xtree_impl_t::remove_node_value(const xp_t& xp)
{
	auto node = get_node_rec(xp);

	return  node.has_value() ? 
		node.value()->value.resize(0),
		true : false;

}

optional<size_t>
xtree_impl_t::get_node_children_count(const xp_t& xp) const
{
	auto node = get_node_rec(xp);

	return node.has_value() ? node.value()->children.size() : optional<size_t>{};
}

optional<bricks_handle_t>
xtree_impl_t::get_node(const xp_t& xp) const
{
	auto node = get_node_rec(xp);

	return node.has_value() ? optional<bricks_handle_t>{ (bricks_handle_t) node.value()} : optional<bricks_handle_t>{};
}

optional<bricks_handle_t>
xtree_impl_t::add_node(const xp_t& xp, bool replicate_leaf)
{
	auto node = get_node_rec(xp,true, replicate_leaf);

	return node.has_value() ? optional<bricks_handle_t>{ (bricks_handle_t)node.value()} : optional<bricks_handle_t>{};
}

bool
xtree_impl_t::set_property_value(const xp_t& xp, const char *property_name, property_value_t v)
{
	auto node = get_node_rec(xp, true, false);

	if (!node.has_value())
		return false;

	for (auto& p : node.value()->properties)
	{
		if (p.first == property_name)
		{
			p.second = v;
			return true;
		}
	};

	node.value()->properties.push_back(make_pair(string(property_name), v));

	return true;
}

optional<property_value_t>
xtree_impl_t::get_property_value(const xp_t& xp, const char *property_name) const
{
	auto node = get_node_rec(xp);

	if (!node.has_value())
		return {};

	for (auto& p : node.value()->properties)
	{
		if (p.first == property_name)
			return p.second;
	}
	
	return {};
}


xtree_impl_t::~xtree_impl_t()
{

}

bool
xtree_impl_t::remove_property(const xp_t& xp, const char *property_name)
{
	auto node = get_node_rec(xp);

	if (!node.has_value())
		return false;

	auto it = find_if(node.value()->properties.begin(), node.value()->properties.end(),
		[&](const property_t& p) { return p.first == property_name; });

	if (it == node.value()->properties.end())
		return false;

	node.value()->properties.erase(it);
	return true;
}


bool
xtree_impl_t::remove_subtree(const xp_t& xp)
{
	auto node = get_node_rec(xp);

	if (!node.has_value())
		return false;

	for (auto it = node.value()->parent->children.begin(); it != node.value()->parent->children.end(); ++it)
	{
		if (&(*it) == node.value())
		{
			node.value()->parent->children.erase(it);
			return true;
		}
	}

	return false;
}

bool
xtree_impl_t::traverse(xtree_visitor_t* v) const
{
	if (genesis.children.size() == 0)
		return true;

	return traverse(genesis.children.front(), v);
}

bool
xtree_impl_t::traverse(const xnode_t& node, xtree_visitor_t* v) const
{
	if (!v->start_element(node.name, node.properties, node.value))
		return false;

	for (auto& child : node.children)
	{
		if (!traverse(child, v))
			return false;
	}
	if (!v->end_element(node.name))
		return false;

	return true;
}

bool
xtree_impl_t::traverse(const xp_t& xp, xtree_visitor_t* v) const
{
	auto node = get_node_rec(xp);

	if (!node.has_value())
		return false;

	return traverse(*node.value(), v);
}