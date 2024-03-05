#include "pch.h"
#include "xml_xtree.h"

xml_xtree_t::~xml_xtree_t() {};

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

bool 
check_path(const char* p)
{
	if (p == nullptr || *p != '/' || *(p + 1) == '\0')
		return false;
	else
		return true;

}

int
xml_xtree_t::get_node_children_count(const char* p) 
{
	if (!check_path(p))
		return -1;

	auto el = walk_path(doc.RootElement(), p + 1);

	if (el)
		return el->ChildElementCount();

	return -1;

};

const char*
xml_xtree_t::get_node_value(const char* np) 
{
	if (!check_path(np))
		return nullptr;

	auto el = walk_path(doc.RootElement(), np + 1);

	if (!el)
		return "";

	return el->GetText();
}

const char*
xml_xtree_t::get_child_name_by_index(const char* np, int i)
{
	if (!check_path(np))
		return nullptr;

	auto el = walk_path(doc.RootElement(), np + 1);

	if (!el)
		return "";

	int curr_i = 0;
	for (XMLElement* child = el->FirstChildElement(); child != nullptr; child = child->NextSiblingElement()) {
		if (curr_i++ == i)		
			return child->Value();
	};

	return "";

}

const char*
xml_xtree_t::get_node_property(const char* p, const char* property)
{
	if (!check_path(p))
		return nullptr;

	auto el = walk_path(doc.RootElement(), p + 1);

	if (!el)
		return "";

	auto  attr = el->FindAttribute(property);

	if (!attr)
		return "";

	return el->FindAttribute(property)->Value();

};

int
xml_xtree_t::get_node_property_as_int(const char* np, const char* pp)
{
	if (!check_path(np))
		return -1;

	auto el = walk_path(doc.RootElement(), np + 1);

	if (!el)
		return -1;

	auto  attr = el->FindAttribute(pp);

	if (!attr)
		return -1;

	int v = -1;
	attr->QueryIntValue(&v);

	return v;
};

bool
xml_xtree_t::get_node_property_as_bool(const char* np, const char* pp)
{
	if (!check_path(np))
		return false;

	auto el = walk_path(doc.RootElement(), np + 1);

	if (!el)
		return false;

	auto  attr = el->FindAttribute(pp);

	if (!attr)
		return false;

	bool v = false;
	attr->QueryBoolValue(&v);

	return v;
};

double
xml_xtree_t::get_node_property_as_double(const char* np, const char* pp)
{
	if (!check_path(np))
		return -1;

	auto el = walk_path(doc.RootElement(), np + 1);

	if (!el)
		return -1;

	auto  attr = el->FindAttribute(pp);

	if (!attr)
		return -1;

	double v = 0.0;
	attr->QueryDoubleValue(&v);

	return v;
};

const char*
xml_xtree_t::get_child_property_by_index(const char* np, int i, const char* pp)
{
	if (!check_path(np))
		return nullptr;

	auto el = walk_path(doc.RootElement(), np + 1);

	if (!el)
		return "";

	int curr_i = 0;
	for (XMLElement* child = el->FirstChildElement(); child != nullptr; child = child->NextSiblingElement()) {
		if (curr_i++ == i) {
			auto  attr = child->FindAttribute(pp);

			if (!attr)
				return "";

			return attr->Value();
		}
			
	};

	return "";

}

int
xml_xtree_t::get_child_property_as_int_by_index(const char* np, int i, const char* pp)
{
	if (!check_path(np))
		return -1;

	auto el = walk_path(doc.RootElement(), np + 1);

	if (!el)
		return -1;

	int curr_i = 0;
	for (XMLElement* child = el->FirstChildElement(); child != nullptr; child = child->NextSiblingElement()) {
		if (curr_i++ == i) {
			auto  attr = child->FindAttribute(pp);

			if (!attr)
				return -1;

			int v = -1;
			attr->QueryIntValue(&v);

			return v;
		}

	};

	return -1;
}

bool
xml_xtree_t::get_child_property_as_bool_by_index(const char* np, int i, const char* pp)
{
	if (!check_path(np))
		return false;

	auto el = walk_path(doc.RootElement(), np + 1);

	if (!el)
		return false;

	int curr_i = 0;
	for (XMLElement* child = el->FirstChildElement(); child != nullptr; child = child->NextSiblingElement()) {
		if (curr_i++ == i) {
			auto  attr = child->FindAttribute(pp);

			if (!attr)
				return false;

			bool v = false;
			attr->QueryBoolValue(&v);

			return v;
		}

	};

	return false;
}

double
xml_xtree_t::get_child_property_as_double_by_index(const char* np, int i, const char* pp)
{
	if (!check_path(np))
		return 0.0;

	auto el = walk_path(doc.RootElement(), np + 1);

	if (!el)
		return 0.0;

	int curr_i = 0;
	for (XMLElement* child = el->FirstChildElement(); child != nullptr; child = child->NextSiblingElement()) {
		if (curr_i++ == i) {
			auto  attr = child->FindAttribute(pp);

			if (!attr)
				return -1;

			double v = 0.0;
			attr->QueryDoubleValue(&v);

			return v;
		}

	};

	return 0.0;
}