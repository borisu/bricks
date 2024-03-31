#pragma once
#include "bricks.h"
#include "xtree.h"


using namespace tinyxml2;
using namespace std;

namespace bricks
{

    class xml_visitor_t: public xtree_visitor_t
    {
    public:

        xml_visitor_t(const xtree_t *xt);

        string serialize();

        virtual bool start_element(const string& name, const property_list_t& properties, const buffer_t& value) override;

        virtual bool end_element(const string& name) override;

    private:

         const xtree_t* xt = nullptr;

         tinyxml2::XMLDocument doc;

         list <tinyxml2::XMLElement*> nodes_stack;

    };

    // Define a class that inherits from XMLVisitor, which is the SAX handler interface
    class SaxHandler : public XMLVisitor 
    {
    public:

        xtree_t* LoadXml(const char* xml);

        // Override the interface's functions to handle events
        virtual bool VisitEnter(const tinyxml2::XMLElement& element, const tinyxml2::XMLAttribute* attribute) override;

        virtual bool VisitExit(const tinyxml2::XMLElement& element) override;

        list<xp_t> nodes_stack;

        xtree_t* xt = nullptr;
       
    };
    
}

