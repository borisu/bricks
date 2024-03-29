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
    tinyxml2::XMLDeclaration* decl = doc.NewDeclaration();
    doc.InsertFirstChild(decl);

    xt->traverse(this);

    // Create a printer
    tinyxml2::XMLPrinter printer;
    // Print the XML document to the printer
    doc.Print(&printer);

    // Get the string representation from the printer
    std::string xmlString = printer.CStr();

    return xmlString;
}

std::string removeTrailingZeros(double value) {
    std::string str = std::to_string(value);
    size_t dotPos = str.find('.');
    size_t curr_pos = str.length() - 1;

    while (curr_pos > dotPos + 1 && str[curr_pos] == '0') {
        str.pop_back();
        curr_pos--;
    }
    
    return str;
}


void 
xml_visitor_t::start_element(const string& name, const properties_list_t& properties, const buffer_t& value)
{
    tinyxml2::XMLElement* child = doc.NewElement(name.c_str());

    if (value.size())
    {
        std::string_view sv(value.data(), value.size());

        string s = base64::to_base64(sv).c_str();

        child->SetText(s.c_str());
    }

   
    for (auto& a : properties)
    {
        if (a.second.type() == typeid(int))
        {
            child->SetAttribute(a.first.c_str(), std::to_string(std::any_cast<int>(a.second)).c_str());
        }
        else if (a.second.type() == typeid(double))
        {
            child->SetAttribute(a.first.c_str(), removeTrailingZeros(std::any_cast<double>(a.second)).c_str());
        }
        else if (a.second.type() == typeid(bool))
        {
            child->SetAttribute(a.first.c_str(), std::any_cast<bool>(a.second) ? "true" : "false");
        }
        else if (a.second.type() == typeid(string))
        {
            child->SetAttribute(a.first.c_str(), std::any_cast<string>(a.second).c_str());
        }
            
    }
    if (nodes_stack.empty())
    {
        doc.InsertEndChild(child);
    }
    else
    {
        nodes_stack.front()->InsertEndChild(child);
    }
    
    nodes_stack.push_front(child);
}

void 
xml_visitor_t::end_element(const string& name)
{
    nodes_stack.pop_front();
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

