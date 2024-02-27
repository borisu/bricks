#include "pch.h"
#include "xml_xtree.h"

/* lib interface bridge */

BRICKS_API bricks_handle_t
bricks_create_xtree_from_xml_file(const char* f)
{
	xtree_t* xt = new xml_xtree_t();
	if ((xt->load(f)) != BRICKS_SUCCESS)
		delete xt;

	return (bricks_handle_t)xt;

}


BRICKS_API bricks_handle_t
bricks_create_xtree_from_xml_string(const char* f)
{
	xtree_t* xt = new xml_xtree_t();
	if ((xt->parse(f)) != BRICKS_SUCCESS)
	{
		delete xt;
		return (bricks_handle_t)nullptr;
	}

	return (bricks_handle_t)xt;

}

BRICKS_API int
bricks_get_node_children_count(bricks_handle_t h, const char* c)
{
	return ((xtree_t*)h)->get_node_children_count(c);
}


/* implementation */

bricks_error_code_e
xml_xtree_t::load(const char* f)
{
	if (doc.LoadFile(f) == XML_SUCCESS)
		return BRICKS_SUCCESS;
	else
		return BRICKS_INVALID_PARAM;

}

bricks_error_code_e
xml_xtree_t::parse(const char* s) 
{
	if (doc.Parse(s) == XML_SUCCESS)
		return BRICKS_SUCCESS;
	else 
		return BRICKS_INVALID_PARAM;

};

XMLElement* 
xml_xtree_t::walk_path(XMLElement* e, const char* p)
{
	// cannot be separator here
	if (*p == '/')
		return nullptr;

	// this is last element ?
	const char* sep_pos = strchr(p, '/');

	auto name_len = (sep_pos == nullptr) ? strlen(p) : sep_pos - p;

	// the name does not correspond to first node in path
	if (strncmp(p, e->Value(), name_len) != 0)
		return nullptr;

	if ((sep_pos ) == nullptr)
		return e;

	for (auto child = e->FirstChildElement(); child != nullptr; child = child->NextSiblingElement()) 
	{
		auto el1 = walk_path(child, sep_pos + 1);
		if (el1 != nullptr)
			return el1;
	}

	return nullptr;
}

int
xml_xtree_t::get_node_children_count(const char* p) 
{

	const char* next_el = nullptr;
	if ((next_el = strchr(p, '/')) != p /*separator must be first*/)
		return -1;

	// last ?
	if (*(next_el + 1) == '\0')
		return 1; 

	auto el = walk_path(doc.RootElement(), next_el + 1);

	if (el)
		return el->ChildElementCount();

	return -1;

};



const char*
xml_xtree_t::get_node_property(const char* c, const char* property) 
{
	return nullptr;
};

int
xml_xtree_t::get_node_property_as_int(const char* c, const char* property) 
{
	return 0;
};

bool
xml_xtree_t::get_node_property_as_bool(const char* c, const char* property)
{
	return false;
};

const char*
xml_xtree_t::get_node_property_by_index(const char* c, int index, const char* property) 
{
	return nullptr;

};


