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
        if (holds_alternative<long long>(a.second))
        {
            child->SetAttribute(a.first.c_str(), ("%L:" + std::to_string(std::get<long long>(a.second))).c_str());
        } 
        else if ( holds_alternative<int>(a.second))
        {
            child->SetAttribute(a.first.c_str(), ("%i:" + std::to_string(std::get<int>(a.second))).c_str());
        }
        else if (holds_alternative<double>(a.second))
        {
            child->SetAttribute(a.first.c_str(),  ("%d:" + removeTrailingZeros(std::get<double>(a.second))).c_str());
        }
        else if (holds_alternative<bool>(a.second))
        {
            child->SetAttribute(a.first.c_str(), std::get<bool>(a.second) ? "%b:true" : "%b:false");
        }
        else if (holds_alternative<string>(a.second))
        {
            child->SetAttribute(a.first.c_str(), ("%s:" + std::get<string>(a.second)).c_str());
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

#define BRICKS_PREFIX_LEN strlen("%x:")




// Override the interface's functions to handle events
bool 
SaxHandler::VisitEnter(const tinyxml2::XMLElement& element, const tinyxml2::XMLAttribute* attribute) {

    auto handle = xt->add_node(nodes_stack.front().set_rel_path(element.Name()),true);

    if (!handle.has_value())
        return false;

    if (element.GetText() != nullptr)
    {
        try
        { 

            string str = base64::from_base64(element.GetText()).c_str();

            std::vector<char> vec(str.begin(), str.end());

            xt->set_node_value(handle.value(), &str[0], (int)str.size());
        }
        catch (std::exception&)
        {
            return false;
        }
    }
    
    // Print the attributes, if any
    if (attribute) {
        const tinyxml2::XMLAttribute* attr = attribute;
        while (attr) {

            optional<property_value_t> p;

            // check enforced types 
            if (strncmp(attr->Value(), "%d:", BRICKS_PREFIX_LEN) == 0)
            {
                size_t pos;
                p = std::stod(attr->Value() + BRICKS_PREFIX_LEN, &pos);
                if (pos != strlen(attr->Value() + BRICKS_PREFIX_LEN))
                    return false;
            }
            else if (strncmp(attr->Value(), "%b:", BRICKS_PREFIX_LEN) == 0)
            {
                if (strcmp(attr->Value() + BRICKS_PREFIX_LEN, "true") == 0)
                    p = true;
                else
                    p = false;
            }
            else if (strncmp(attr->Value(), "%i:", BRICKS_PREFIX_LEN) == 0)
            {
                size_t pos;
                p = std::stoi(attr->Value()+ BRICKS_PREFIX_LEN, &pos);
                if (pos != strlen(attr->Value() + BRICKS_PREFIX_LEN))
                    return false;
            }
            else if (strncmp(attr->Value(), "%x:", BRICKS_PREFIX_LEN) == 0)
            {
                size_t pos;
                p = (int)std::stol(attr->Value() + BRICKS_PREFIX_LEN, &pos,16);
                if (pos != strlen(attr->Value() + BRICKS_PREFIX_LEN))
                    return false;
            }
            else if (strncmp(attr->Value(), "%L:", BRICKS_PREFIX_LEN) == 0)
            {
                size_t pos;
                p = std::stoll (attr->Value() + BRICKS_PREFIX_LEN, &pos);
                if (pos != strlen(attr->Value() + BRICKS_PREFIX_LEN))
                    return false;
            }
            else if (strncmp(attr->Value(), "%X:", BRICKS_PREFIX_LEN) == 0)
            {
                size_t pos;
                p = std::stoll(attr->Value() + BRICKS_PREFIX_LEN, &pos,16);
                if (pos != strlen(attr->Value() + BRICKS_PREFIX_LEN))
                    return false;
            }
            else if (strncmp(attr->Value(), "%s:", BRICKS_PREFIX_LEN) == 0)
            {
                p = string(attr->Value() + BRICKS_PREFIX_LEN);
            }
            else // no default values
            {
                try {
                    if (strchr(attr->Value(), '.') != nullptr)
                    {
                        size_t pos;
                        p = std::stod(attr->Value(), &pos);
                        if (pos != strlen(attr->Value()))
                            p = {};

                    }
                }
                catch (std::exception&) {}

                if (!p.has_value())
                {
                    try {
                        size_t pos;
                        p = std::stoi(attr->Value(), &pos);
                        if (pos != strlen(attr->Value()))
                            p = {};
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
            }

            xt->set_property_value(handle.value(), attr->Name(), p.value());

            attr = attr->Next();
        }
    };

    nodes_stack.push_front(xp_t(handle.value()));

    return true; // Continue visiting
}

