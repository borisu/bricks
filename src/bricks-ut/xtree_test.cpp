#include "pch.h"
#include "xtree_api.h"

TEST(xtree_case, xtree_load_xml_test) 
{
	auto h = create_xtree();

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
	

	//EXPECT_EQ(h->get_node_children_count("/").value(), 1);
	//EXPECT_EQ(h->get_node_children_count("/configuration").value(), 4);

	EXPECT_EQ(h->get_node_children_count("/configuration/property").value(), 1);
	EXPECT_EQ(h->get_node_children_count("/configuration/property/subproperty").value(), 0);

	/*EXPECT_EQ(bricks_get_children_count(h, "/a/b/c"), 0);
	EXPECT_EQ(bricks_get_children_count(h, "/a"), 3);
	EXPECT_EQ(bricks_get_children_count(h, "/a/b"), 1);*/
 	
}


TEST(xtree_case, xtree_direct_access)
{
	auto h = create_xtree();

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
	EXPECT_EQ(h->get_node_children_count(c.value(), "/property").value(), 1);

}

/*
TEST(xtree_case, xtree_parse_xml_test)
{
	
	
}*/