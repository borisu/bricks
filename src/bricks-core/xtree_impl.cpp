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
	root.name = "/";
};


optional<xtree_impl_t::xnode_t*>
xtree_impl_t::get_node_rec1(xnode_t& node, const string_view& path, bool create, bool replicate_leaf)
{
	auto start = path.find_first_not_of('/'); // no more valid node name characters
	if (start == string_view::npos)
	{
		return &node == &root ? optional<xtree_impl_t::xnode_t*>{} : &node ; // never return hidden root as valid path query result 
	}

	auto end = path.find_first_of('/', start);
	if (end == string_view::npos)
	{
		end = path.length();
	}
	

	// get view on child name
	string_view child_name = string_view(&path[start], end - start);

	auto it = find_if(node.children.begin(), node.children.end(),
		[&](const xnode_t &child) { return child.name == child_name; });

	xtree_impl_t::xnode_t* child = nullptr;

	bool already_created = false;
	if (it == node.children.end())
	{
		if (create)
		{
			xnode_t new_child;

			new_child.name = child_name;
			new_child.parent = &node;

			node.children.push_back(new_child);
			child = &node.children.back();
			already_created = true;
		}
		else
		{
			return optional<xtree_impl_t::xnode_t*> {};
		}

	}
	else
	{
		child = &(*it);
	}


	//MSVC does not create string_view with zero length path so we need special handling for that
	auto ret = get_node_rec1(*child, end == path.length() ? "" : string_view(&path[end], path.length() - end), create);

	if (ret.has_value() && ret.value() == child) // same value means child is a leaf
	{
		if (replicate_leaf && !already_created)
		{
			if (&node == &root)
			{
				return optional<xtree_impl_t::xnode_t*>{}; // cannot add more than one child to root node
			};

			// otherwise replicate the child
			xnode_t new_child;

			new_child.name = child_name;
			new_child.parent = &node;

			node.children.push_back(new_child);
			return optional<xtree_impl_t::xnode_t*> {&node.children.back()};
		}

	}

	return ret;

}

optional<xtree_impl_t::xnode_t*>
xtree_impl_t::get_node_rec(const xnode_t& node, const string_view& path) const
{
	return  const_cast<xtree_impl_t*>(this)->get_node_rec1(const_cast<xnode_t&>(node), path, false);
}

optional<bricks_handle_t>
xtree_impl_t::set_node_value(optional<xtree_impl_t::xnode_t*> node, const char* buf, int len)
{
	if (!node.has_value())
		return {};


	node.value()->value.resize(len);
	node.value()->value.assign(buf, buf + len);

	return  (bricks_handle_t)node.value();
}

optional<bricks_handle_t>
xtree_impl_t::set_node_value(const string_view& path, const char* buf, int len, bool create)
{
	return set_node_value(get_node_rec1(root, path, create), buf, len);
}

optional<bricks_handle_t>
xtree_impl_t::set_node_value(bricks_handle_t h, const string_view& path, const char* buf, int len, bool create )
{
	return set_node_value(get_node_rec1(*((xnode_t*)h), path, create), buf, len);
}

optional<const buffer_t*>
xtree_impl_t::get_node_value(const string_view& path) const
{
	auto node = get_node_rec(root, path);

	return node.has_value() ? &(node.value()->value) : optional<const buffer_t*>{};
}

optional<size_t>
xtree_impl_t::get_node_children_count(const string_view& path) const
{
	auto node = get_node_rec(root, path);

	return node.has_value() ? node.value()->children.size() : optional<size_t>{};
}

optional<bricks_handle_t>
xtree_impl_t::add_node(bricks_handle_t h, const string_view& path, bool leaf_replicator)
{
	xnode_t* pnode = (xnode_t*)h;

	auto node = get_node_rec1(*pnode, path, true, leaf_replicator);

	return node.has_value() ? (bricks_handle_t)node.value() : optional<bricks_handle_t>{};
}

// opaque node handle
optional<bricks_handle_t>
xtree_impl_t::get_node(const string_view& path) const
{
	auto node = get_node_rec(root, path);
	return node.has_value() ? optional<bricks_handle_t>{ (bricks_handle_t) node.value()} : optional<bricks_handle_t>{};
}

optional<bricks_handle_t>
xtree_impl_t::add_node(const string_view& path, bool leaf_replicator)
{
	auto node = get_node_rec1(root, path,true, leaf_replicator);
	return node.has_value() ? optional<bricks_handle_t>{ (bricks_handle_t)node.value()} : optional<bricks_handle_t>{};
}

optional<xtree_impl_t::xnode_t*>
xtree_impl_t::get_child(optional<xnode_t*> node, int index) const
{
	if (!node.has_value())
		return optional<xnode_t*>{};

	if (node.value()->children.size() < index + 1)
		return optional<xnode_t*>{};

	auto it = node.value()->children.begin();
	std::advance(it, index);

	return &(*it);
}

optional<xtree_impl_t::xnode_t*>
xtree_impl_t::get_child(const string_view& path, int index) const
{
	return get_child(get_node_rec(root, path), index);
}

optional<xtree_impl_t::xnode_t*>
xtree_impl_t::get_child(bricks_handle_t h, const string_view& path, int index) const
{
	return get_child(get_node_rec(*(xnode_t*)h, path), index);
}

optional<size_t>
xtree_impl_t::get_node_children_count(bricks_handle_t h, const string_view& path) const
{
	xnode_t* pnode = (xnode_t*)h;

	auto node = get_node_rec(*pnode, path);

	return node.has_value() ? node.value()->children.size() : optional<size_t>{};
}

optional<bricks_handle_t>
xtree_impl_t::get_node(bricks_handle_t h, const string_view& path) const
{
	xnode_t* pnode = (xnode_t*)h;

	auto node = get_node_rec(*pnode, path);

	return node.has_value() ? (bricks_handle_t)node.value() : optional<bricks_handle_t>{};

}

optional<string>
xtree_impl_t::get_child_name_by_index(const string_view& path, int i) const
{
	auto node = get_child(path, i);

	if (!node.has_value())
		return optional<string>{};

	return node.value()->name;
}

optional<string>
xtree_impl_t::get_child_name_by_index(bricks_handle_t h, const string_view& path, int i) const
{
	auto node = get_child(h, path, i);

	if (!node.has_value())
		return optional<string>{};

	return node.value()->name;
}

template<typename T>
optional<T>
xtree_impl_t::get_child_property_value_as(const string_view& path, int index, const string_view& property_name) const
{
	return get_property_value_as1<T>(get_child(path, index), property_name);
}

template<typename T>
optional<T>
xtree_impl_t::get_property_value_as(const string_view& path, const string_view& property_name) const
{
	return get_property_value_as1<T>(get_node_rec(root, path), property_name);
}

template<typename T>
optional<T>	
xtree_impl_t::get_property_value_as1(optional<xtree_impl_t::xnode_t*> node, const string_view& property_name) const
{

	if (!node.has_value())
		return optional<T>{};

	try
	{
		for (auto& p : node.value()->properties)
		{
			if (p.first == property_name)
				return optional<T>{std::any_cast<T>(p.second)};
		}
	}
	catch (const std::bad_any_cast&)
	{

	}

	return optional<T>{};
}

bool
xtree_impl_t::set_property_value(optional<xtree_impl_t::xnode_t*> node, const string_view& property_name, std::any	v)
{
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

bool
xtree_impl_t::set_property_value(const string_view& path, const string_view& property_name, int v, bool create)
{
	return set_property_value(get_node_rec1(root, path, create), property_name, v);
}

bool
xtree_impl_t::set_property_value(const string_view& path, const string_view& property_name, bool v, bool create)
{
	return set_property_value(get_node_rec1(root, path, create), property_name, v);
}

bool
xtree_impl_t::set_property_value(const string_view& path, const string_view& property_name, double v, bool create)
{
	return set_property_value(get_node_rec1(root, path, create), property_name, v);
}

bool
xtree_impl_t::set_property_value(const string_view& path, const string_view& property_name, const string_view& v, bool create)
{
	return set_property_value(get_node_rec1(root, path, create), property_name, string(v));
}

bool
xtree_impl_t::set_property_value(bricks_handle_t node, const string_view& path, const string_view& property_name, int v, bool create)
{
	return set_property_value(get_node_rec1(root, path, create), property_name, v);
}

bool
xtree_impl_t::set_property_value(bricks_handle_t node, const string_view& path, const string_view& property_name, bool v, bool create)
{
	return set_property_value(get_node_rec1(*(xnode_t*)node, path, create), property_name, v);
}

bool
xtree_impl_t::set_property_value(bricks_handle_t node, const string_view& path, const string_view& property_name, double v, bool create)
{
	return set_property_value(get_node_rec1(*(xnode_t*)node, path, create), property_name, v);
}

bool
xtree_impl_t::set_property_value(bricks_handle_t node, const string_view& path, const string_view& property_name, const string_view& v, bool create)
{
	return set_property_value(get_node_rec1(*(xnode_t*)node, path, create), property_name, string(v));
}

optional<string>
xtree_impl_t::get_property_value_as_string(const string_view& path, const string_view& property_name) const
{
	return get_property_value_as<string>(path, property_name);
}

optional<int>
xtree_impl_t::get_property_value_as_int(const string_view& path, const string_view& property_name) const
{
	return get_property_value_as<int>(path, property_name);
}

optional<bool>
xtree_impl_t::get_property_value_as_bool(const string_view& path, const string_view& property_name) const
{
	return get_property_value_as<bool>(path, property_name);
}

optional<double>
xtree_impl_t::get_property_value_as_double(const string_view& path, const string_view& property_name) const
{
	return get_property_value_as<double>(path, property_name);
}

optional<string>
xtree_impl_t::get_child_property_value_as_string(const string_view& path, int i, const string_view& property_name) const
{
	return get_child_property_value_as<string>(path, i, property_name);
}

optional<int>
xtree_impl_t::get_child_property_value_as_int(const string_view& path, int i, const string_view& property_name) const
{
	return get_child_property_value_as<int>(path, i, property_name);
}

optional<bool>
xtree_impl_t::get_child_property_value_as_bool(const string_view& path, int i, const string_view& property_name) const
{
	return get_child_property_value_as<bool>(path, i, property_name);
}

optional<double>
xtree_impl_t::get_child_property_value_as_double(const string_view& path, int i, const string_view& property_name) const
{
	return get_child_property_value_as<double>(path, i, property_name);
}

xtree_impl_t::~xtree_impl_t()
{

}

void
xtree_impl_t::remove_property(optional<xtree_impl_t::xnode_t*> node, const string_view& property_name)
{
	if (!node.has_value())
		return;

	auto it = find_if(node.value()->properties.begin(), node.value()->properties.end(),
		[&](const property_t& p) { return p.first == property_name; });

	if (it == node.value()->properties.end())
		return;

	node.value()->properties.erase(it);
}

void
xtree_impl_t::remove_property(const string_view& path, const string_view& property_name)
{
	remove_property(get_node_rec1(root, path, false), property_name);
}

void
xtree_impl_t::remove_property(bricks_handle_t h, const string_view& path, const string_view& property_name)
{
	remove_property(get_node_rec1(*((xnode_t*)h), path, false), property_name);
}

void
xtree_impl_t::remove_value(optional<xtree_impl_t::xnode_t*> node)
{
	if (!node.has_value())
		return;

	node.value()->value.resize(0);

}

void
xtree_impl_t::remove_node_value(const string_view& path)
{
	remove_value(get_node_rec1(root, path, false));
}

void
xtree_impl_t::remove_node_value(bricks_handle_t h, const string_view& path)
{
	remove_value(get_node_rec1(*((xnode_t*)h), path, false));
}

void
xtree_impl_t::remove_subtree(optional<xtree_impl_t::xnode_t*> node)
{
	if (!node.has_value())
		return;

	if (node.value() == &root)
		return;

	for (auto it = node.value()->parent->children.begin(); it != node.value()->parent->children.end(); ++it)
	{
		if (&(*it) == node.value())
		{
			node.value()->parent->children.erase(it);
			break;
		}
	}
}
 
void
xtree_impl_t::remove_subtree(const string_view& path)
{
	remove_subtree(get_node_rec1(root, path, false));
}

void
xtree_impl_t::remove_subtree(bricks_handle_t h, const string_view& path)
{
	remove_subtree(get_node_rec1(*((xnode_t*)h), path, false));
}

void
xtree_impl_t::traverse(xtree_visitor_t* v) const
{
	if (root.children.size() == 0)
		return;

	traverse(v, root.children.front());
}

void
xtree_impl_t::traverse(const string_view& path, xtree_visitor_t* v) const
{
	traverse(get_node_rec(root, path),v);
}

void
xtree_impl_t::traverse(bricks_handle_t h, const string_view& path, xtree_visitor_t *v) const
{
	traverse(get_node_rec(*((xnode_t*)h), path), v);
}

void
xtree_impl_t::traverse(optional<xtree_impl_t::xnode_t*> node, xtree_visitor_t* v) const
{
	if (!node.has_value())
		return;

	traverse(v, *node.value());
}

void
xtree_impl_t::traverse(xtree_visitor_t* v, const xnode_t& e) const
{
	v->start_element(e.name, e.properties, e.value);
	for (auto& child : e.children)
	{
		traverse(v, child);
	}
	v->end_element(e.name);

}