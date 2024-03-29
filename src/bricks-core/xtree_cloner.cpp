#include "xtree_cloner.h"

using namespace bricks;



xtree_cloner_t::xtree_cloner_t(){}

bool 
xtree_cloner_t::start_element(const string& name, const properties_list_t& properties, const buffer_t& value) 
{
    optional<bricks_handle_t> h;
    if (dst_handles.size() == 0)
    {
        h = dst->add_node(name, true);
    }
    else
    {
        h = dst->add_node(dst_handles.back(), name, true);
    }

	if (!h.has_value())
		return false;

	for (auto& a : properties)
	{
        if (a.second.type() == typeid(int))
        {
            dst->set_property_value(h.value(), "", a.first, std::any_cast<int>(a.second));
            
        }
        else if (a.second.type() == typeid(double))
        {
            dst->set_property_value(h.value(), "", a.first, std::any_cast<double>(a.second));
        }
        else if (a.second.type() == typeid(bool))
        {
            dst->set_property_value(h.value(), "", a.first, std::any_cast<bool>(a.second));

        }
        else if (a.second.type() == typeid(string))
        {
            dst->set_property_value(h.value(), "", a.first, std::any_cast<string>(a.second));
        }
		
	}

	dst_handles.push_back(h.value());

    return true;
};

bool 
xtree_cloner_t::end_element(const string& name) 
{
	dst_handles.pop_back();

    return true;
};

bool 
xtree_cloner_t::clone(xtree_t* src, optional<bricks_handle_t> src_h, const string_view& path, xtree_t* dst, optional<bricks_handle_t> dst_h)
{
    this->dst = dst;

    if (dst_h.has_value())
        dst_handles.push_back(dst_h.value());

    if (src_h.has_value())
    {
        return src->traverse(src_h.value(), path, this);
    }
    else
    {
        return src->traverse(this);
    }
}

bool
bricks::clone_xtree(xtree_t* src, optional<bricks_handle_t> src_h, const string_view& path, xtree_t* dst, optional<bricks_handle_t> dst_h)
{
    return xtree_cloner_t().clone(src, src_h, path, dst, dst_h);
}

