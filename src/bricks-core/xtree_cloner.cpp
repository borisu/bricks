#include "xtree_cloner.h"

using namespace bricks;

xtree_cloner_t::xtree_cloner_t(const xtree_t* src, const xp_t& xp_src, xtree_t* dst, const xp_t& xp_dst):src(src),xp_src(xp_src),dst(dst),xp_dst(xp_dst)
{

}

bool 
xtree_cloner_t::start_element(const string& name, const property_list_t& properties, const buffer_t& value) 
{
    
    auto h = dst->add_node(dst_handles.back().set_rel_path(name.c_str()), true);
    

	if (!h.has_value())
		return false;

	for (auto& a : properties)
	{
        if (holds_alternative<int>(a.second))
        {
            if (!dst->set_property_value(h.value(), a.first.c_str(), std::get<int>(a.second)))
                return false;
        }
        else if (holds_alternative<double>(a.second))
        {
            if (!dst->set_property_value(h.value(),  a.first.c_str(), std::get<double>(a.second)))
                return false;
        }
        else if (holds_alternative<bool>(a.second))
        {
            if (!dst->set_property_value(h.value(),  a.first.c_str(), std::get<bool>(a.second)))
                return false;
        }
        else if (holds_alternative<string>(a.second))
        {
            if (!dst->set_property_value(h.value(),  a.first.c_str(), std::get<string>(a.second)))
                return false;
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
xtree_cloner_t::clone()
{
    dst_handles.push_back(xp_dst);

    return src->traverse(xp_src, this);
}


bool 
bricks::clone_xtree(const xtree_t* src, const xp_t& xp_src, xtree_t* dst, const xp_t& xp_dst)
{
    return xtree_cloner_t(src,xp_src,dst,xp_dst).clone();
}

