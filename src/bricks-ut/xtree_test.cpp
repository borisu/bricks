#include "pch.h"
#include "xtree_api.h"

TEST(xtree_case, xtree_load_xml_test) 
{

	auto h = bricks_create_xtree_from_xml_file("test1.xml");

 	EXPECT_NE(h, 0);

	EXPECT_EQ(bricks_get_node_children_count(h, "/"), -1);
	EXPECT_EQ(bricks_get_node_children_count(h, "/a/b/c"), 0);
	EXPECT_EQ(bricks_get_node_children_count(h, "/a"), 3);
	EXPECT_EQ(bricks_get_node_children_count(h, "/a/b"), 1);
 	
}

TEST(xtree_case, xtree_read_children)
{
	auto h =
		bricks_create_xtree_from_xml_string(
			"<kafkaconfiguration>"
			" <property name = \"bool.value\"   value=\"true\" />"
			" <property name = \"int.value\"    value=\"5\" />"
			" <property name = \"double.value\" value=\"5.0\" />"
			" <property name = \"str.value\"    value=\"Mother do you think\" />"
			"</kafkaconfiguration>"
		);

	int c = 0;
	c = bricks_get_node_children_count(h, "/kafkaconfiguration");
	EXPECT_EQ(c, 4);

	EXPECT_STREQ(bricks_get_child_value_by_index(h, "/kafkaconfiguration",0), "property");
	EXPECT_EQ(bricks_get_child_property_as_bool_by_index(h,"/kafkaconfiguration",0,"value"), true);

	EXPECT_STREQ(bricks_get_child_value_by_index(h, "/kafkaconfiguration", 1), "property");
	EXPECT_STREQ(bricks_get_child_value_by_index(h, "/kafkaconfiguration", 2), "property");
	EXPECT_STREQ(bricks_get_child_value_by_index(h, "/kafkaconfiguration", 3), "property");
	EXPECT_STREQ(bricks_get_child_value_by_index(h, "/kafkaconfiguration", 4), "");

	


	EXPECT_NE(h, 0);



}

TEST(xtree_case, xtree_parse_xml_test)
{
	auto h = bricks_create_xtree_from_xml_string("<a/>");

	EXPECT_NE(h, 0);

	EXPECT_EQ(bricks_get_node_children_count(h, "/a"), 0);
	EXPECT_EQ(bricks_get_node_children_count(h, "/"), -1);
	
}