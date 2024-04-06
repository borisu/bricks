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


bool 
xml_visitor_t::start_element(const string& name, const property_list_t& properties, const vector_t& value)
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
        if ( holds_alternative<int>(a.second))
        {
            child->SetAttribute(a.first.c_str(), std::to_string(std::get<int>(a.second)).c_str());
        }
        else if (holds_alternative<double>(a.second))
        {
            child->SetAttribute(a.first.c_str(), removeTrailingZeros(std::get<double>(a.second)).c_str());
        }
        else if (holds_alternative<bool>(a.second))
        {
            child->SetAttribute(a.first.c_str(), std::get<bool>(a.second) ? "true" : "false");
        }
        else if (holds_alternative<string>(a.second))
        {
            child->SetAttribute(a.first.c_str(), std::get<string>(a.second).c_str());
        }
        else
        {
            return false;
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
    return true;
}

bool 
xml_visitor_t::end_element(const string& name)
{
    nodes_stack.pop_front();
    return true;
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

    xt = create_xtree();

    if (doc.Parse(xml) != tinyxml2::XML_SUCCESS) {
        xt->release();
        return nullptr;
    }

    nodes_stack.push_front(xp_t{});

    // Traverse the XML document using SAX
    if (!doc.Accept(this)) {
        xt->release();
        return nullptr;
    }

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

    auto handle = xt->add_node(nodes_stack.front().set_rel_path(element.Name()),true);

    if (!handle.has_value())
        return false;

    if (element.GetText() != nullptr)
    {
        string str = base64::from_base64(element.GetText()).c_str();

        std::vector<char> vec(str.begin(), str.end());

        xt->set_node_value(handle.value(), &str[0], (int)str.size());
    }
    
    // Print the attributes, if any
    if (attribute) {
        const tinyxml2::XMLAttribute* attr = attribute;
        while (attr) {

            optional<property_value_t> p;

            try {
                if (strchr(attr->Value(), '.') != nullptr)
                {
                    p = std::stod(attr->Value(), nullptr);
                }
            }
            catch (std::exception&) {}

            if (!p.has_value())
            {
                try {
                    p = std::stoi(attr->Value(), nullptr);
                }
                catch (std::exception&) {}
            }

            if (!p.has_value())
            {
                if (strcmp(attr->Value(), "true") == 0)
                    p = true;
            }

            if (!p.has_value())
            {
                if (strcmp(attr->Value(), "false") == 0)
                    p = false;
            }

            if (!p.has_value())
            {
                p = string(attr->Value());
            }

            xt->set_property_value(handle.value(), attr->Name(), p.value());

            attr = attr->Next();
        }
    };

    nodes_stack.push_front(xp_t(handle.value()));

    return true; // Continue visiting
}

