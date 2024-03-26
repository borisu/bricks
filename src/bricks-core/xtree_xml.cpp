#include "pch.h"
#include "xtree_xml.h"

using namespace std;
using namespace bricks;

string   
bricks::serialize_xtree_to_xml(const xtree_t *xt)
{
    return xml_visitor_t(xt).serialize();//0..
}

xml_visitor_t::xml_visitor_t(const xtree_t* xt):xt(xt)
{

}

string
xml_visitor_t::serialize() 
{
    xt->traverse(this);
}

void 
xml_visitor_t::start_element(const string& name, const map<string, any>& properties, const buffer_t& value)
{
    tinyxml2::XMLElement* child = doc.NewElement(name.c_str());
    

    for (auto& a : properties)
    {
        switch (a.second.type())
        {
            case typeid(int) : { child2->SetAttribute( return std::to_string<int>(value); }

        }
            
    }

    doc.InsertEndChild(root);
}

void 
xml_visitor_t::end_element(const string& name)
{

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


BRICKS_API xtree_t*
bricks::create_xtree_from_xml(const char* xml)
{
    return  SaxHandler().LoadXml(xml);
}

xtree_t* 
SaxHandler::LoadXml(const char* xml)
{
    tinyxml2::XMLDocument doc; 

    auto xt = create_xtree();

    if (doc.Parse(xml) != tinyxml2::XML_SUCCESS) {
        xt->release();
        return nullptr;
    }

    // Traverse the XML document using SAX
    if (!doc.Accept(this)) {
        xt->release();
        return nullptr;
    }

    auto root = xt->get_node("/");

    if (!root.has_value())
    {
        xt->release();
        return nullptr;
    }
    
    nodes_stack.push_front(root.value());

    return xt;
}

bool 
SaxHandler::VisitExit(const tinyxml2::XMLElement& element)
{
    nodes_stack.pop_front();
    return true;
}

// Override the interface's functions to handle events
bool 
SaxHandler::VisitEnter(const tinyxml2::XMLElement& element, const tinyxml2::XMLAttribute* attribute) {

    auto handle = xt->add_node(nodes_stack.front(), element.Name());

    if (!handle.has_value())
        return false;
    
    // Print the attributes, if any
    if (attribute) {
        const tinyxml2::XMLAttribute* attr = attribute;
        while (attr) {
            xt->set_property_value(handle.value(), "", attr->Name(), attr->Value());
            attr = attr->Next();
        }
    }



    return true; // Continue visiting
}

