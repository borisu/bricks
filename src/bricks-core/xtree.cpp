#include "pch.h"
#include "xtree.h"



xtree_impl_t::xtree_impl_t() 
{
	root.name = "/";
};

bricks_error_code_e
xtree_impl_t::load_from_xml(const char* str)
{
	if (doc.Parse(str) != tinyxml2::XML_SUCCESS) {
		return BRICKS_INVALID_PARAM;
	}

	// Get the root element
	return traverse_elements(doc.RootElement(), root);
}

bricks_error_code_e
xtree_impl_t::traverse_elements(const tinyxml2::XMLElement* element, xnode_t& parent_node)
{

	parent_node.children.push_back(xnode_t{});

	auto& node = parent_node.children.back();

	node.name = element->Name();

	const tinyxml2::XMLAttribute* attr = element->FirstAttribute();
	while (attr) {
		node.properties[attr->Name()] = string(attr->Value());
		attr = attr->Next();
	}


	// Recursively traverse child elements
	for (const tinyxml2::XMLElement* child = element->FirstChildElement(); child != nullptr; child = child->NextSiblingElement()) {
		traverse_elements(child, node);
	}
	return  BRICKS_SUCCESS;
}

optional<xtree_impl_t::xnode_t*>
xtree_impl_t::get_node_rec1(xnode_t& node, const string_view& path, bool create)
{
	if (path.length() > 0)
	{
		// we are at leaf?
		size_t end = path.find_first_of('/');
		if (end == 0)
		{
			//MSVC does not create string_view with zero length path so we need special handling for that
			return get_node_rec1(node, path.length() == 1 ? "" : string_view(&path[1], path.length() - 1),create);
		}

		auto child_name_len = end == string_view::npos ? path.length() : end;

		// get view on child name
		string_view child_name_view = string_view(&path[0], child_name_len);


		auto it = find_if(node.children.begin(), node.children.end(),
			[&child_name_view](const xnode_t child) { return child.name == child_name_view; });

		if (it == node.children.end())
		{
			if (create)
			{
				xnode_t new_child;
				new_child.name = child_name_view;
				node.children.push_back(new_child);
				it = --node.children.end();
			}
			else
			{
				return optional<xtree_impl_t::xnode_t*> {};
			}

		}

		//MSVC does not create string_view with zero length path so we need special handling for that
		return get_node_rec1(*it, child_name_len == path.length() ? "" : string_view(&path[child_name_len], path.length() - child_name_len), create);

	};

	// we are at destination node
	return &node;

}

optional<xtree_impl_t::xnode_t*>
xtree_impl_t::get_node_rec(const xnode_t& node, const string_view& path) const
{
	return  const_cast<xtree_impl_t*>(this)->get_node_rec1(const_cast<xnode_t&>(node), path, false);

}

optional<bricks_handle_t>
xtree_impl_t::set_node_value(const string& path, const char* buf, size_t len, bool create)
{
	auto node = get_node_rec1(root, path, create);
	if (!node.has_value())
		return {};

	node.value()->value.assign(buf, buf + len);

	return  (bricks_handle_t)node.value();
}


optional<bricks_handle_t>
xtree_impl_t::set_node_value(const string& path, const buffer_t* buf, bool create)
{
	auto node = get_node_rec1(root, path, create);
	if (!node.has_value())
		return {};

	node.value()->value = *buf;

	return  (bricks_handle_t)node.value();
}

optional<const buffer_t*>
xtree_impl_t::get_node_value(const string& path) const
{
	auto node = get_node_rec(root, path);

	return node.has_value() ? &(node.value()->value) : optional<const buffer_t*>{};
}

optional<size_t>
xtree_impl_t::get_node_children_count(const string& path) const
{
	auto node = get_node_rec(root, path);

	return node.has_value() ? node.value()->children.size() : optional<size_t>{};
}

optional<size_t>
xtree_impl_t::get_node_children_count(bricks_handle_t h, const string& path) const
{
	xnode_t* pnode = (xnode_t*)h;

	auto node = get_node_rec(*pnode, path);

	return node.has_value() ? node.value()->children.size() : optional<size_t>{};
}

// opaque node handle
optional<bricks_handle_t>
xtree_impl_t::get_node(const string& path) const
{
	auto node = get_node_rec(root, path);
	return node.has_value() ? optional<bricks_handle_t>{ (bricks_handle_t) node.value()} : optional<bricks_handle_t>{};
}

optional<xtree_impl_t::xnode_t*>
xtree_impl_t::get_child(const string& path, int index) const
{
	auto node = get_node_rec(root, path);

	if (!node.has_value())
		return optional<xnode_t*>{};

	if (node.value()->children.size() < index + 1)
		return optional<xnode_t*>{};

	auto it = node.value()->children.begin();
	std::advance(it, index);

	return &(*it);
}

optional<string>
xtree_impl_t::get_child_name_by_index(const string& path, int i) const
{
	auto node = get_child(path, i);

	if (!node.has_value())
		return optional<string>{};

	return node.value()->name;
}

template<typename T>
optional<T>	
xtree_impl_t::get_property_value_as(const string& path, const string& property_name) const
{
	auto node = get_node_rec(root, path);

	if (!node.has_value())
		return optional<T>{};

	auto it = node.value()->properties.find(property_name);
	if (it == node.value()->properties.end())
	{
		return optional<T>{};
	}

	try
	{
		return optional<T>{std::any_cast<T>(it->second)};
	}
	catch (const std::bad_any_cast&)
	{
		return optional<T>{};
	}
}

template<typename T>
optional<T>
xtree_impl_t::get_child_property_value_as(const string& path, int index, const string& property_name) const
{
	auto node = get_child(path, index);

	if (!node.has_value())
		return optional<T>{};

	auto it = node.value()->properties.find(property_name);
	if (it == node.value()->properties.end())
	{
		return optional<T>{};
	}

	try
	{
		return optional<T>{std::any_cast<T>(it->second)};
	}
	catch (const std::bad_any_cast& )
	{
		return optional<T>{};
	}
}

optional<string>
xtree_impl_t::get_property_value_as_string(const string& path, const string& property_name) const
{
	return get_property_value_as<string>(path, property_name);
}

optional<int>
xtree_impl_t::get_property_value_as_int(const string& path, const string& property_name) const
{
	return get_property_value_as<int>(path, property_name);
}

optional<bool>
xtree_impl_t::get_property_value_as_bool(const string& path, const string& property_name) const
{
	return get_property_value_as<bool>(path, property_name);
}

optional<double>
xtree_impl_t::get_property_value_as_double(const string& path, const string& property_name) const
{
	return get_property_value_as<double>(path, property_name);
}

optional<string>
xtree_impl_t::get_child_property_value_as_string(const string& path, int i, const string& property_name) const
{
	return get_child_property_value_as<string>(path, i, property_name);
}

optional<int>
xtree_impl_t::get_child_property_value_as_int(const string& path, int i, const string& property_name) const
{
	return get_child_property_value_as<int>(path, i, property_name);
}

optional<bool>
xtree_impl_t::get_child_property_value_as_bool(const string& path, int i, const string& property_name) const
{
	return get_child_property_value_as<bool>(path, i, property_name);
}

optional<double>
xtree_impl_t::get_child_property_value_as_double(const string& path, int i, const string& property_name) const
{
	return get_child_property_value_as<double>(path, i, property_name);
}

xtree_impl_t::~xtree_impl_t()
{

}



 