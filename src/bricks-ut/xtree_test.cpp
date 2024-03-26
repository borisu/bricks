#include "pch.h"
#include "xtree.h"

using namespace bricks;

TEST(xtree_case, xtree_load_xml_test) 
{
	/*auto h = create_xtree();

	EXPECT_EQ(BRICKS_SUCCESS, h->load_from_xml(
		"<configuration>"
		" <property name = \"bool.value\"   value=\"true\">"
		"    <subproperty/>"
		" </property>"
		" <property name = \"int.value\"    value=\"5\" />"
		" <property name = \"double.value\" value=\"5.0\" />"
		" <property name = \"str.value\"    value=\"Mother do you think\" />"
		"</configuration>"
	));
	

	
	EXPECT_EQ(h->get_node_children_count("/configuration/property").value(), 1);
	EXPECT_EQ(h->get_node_children_count("/configuration/property/subproperty").value(), 0);
	
	EXPECT_EQ(h->get_child_property_value_as_string("/configuration", 0, "name").value(), "bool.value");
	EXPECT_EQ(h->get_child_property_value_as_string("/configuration", 0, "value").value(), "true");*/
 	
}

TEST(xtree_case, xtree_create)
{
	auto xt = create_xtree();
}


TEST(xtree_case, xtree_direct_access)
{
	/*auto h = create_xtree();

	EXPECT_EQ(BRICKS_SUCCESS, h->load_from_xml(
		"<configuration>"
		" <property name = \"bool.value\"   value=\"true\">"
		"    <subproperty/>"
		" </property>"
		" <property name = \"int.value\"    value=\"5\" />"
		" <property name = \"double.value\" value=\"5.0\" />"
		" <property name = \"str.value\"    value=\"Mother do you think\" />"
		"</configuration>"
	));

	auto c = h->get_node("/configuration");
	EXPECT_EQ(h->get_node_children_count(c.value(), "").value(), 4);
	EXPECT_EQ(h->get_node_children_count(c.value(), "/").value(), 4);
	EXPECT_EQ(h->get_node_children_count(c.value(), "/property").value(), 1);*/

}
