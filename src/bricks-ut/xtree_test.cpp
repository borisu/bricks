#include "pch.h"
#include "xtree_api.h"

TEST(xtree_case, xtree_load_xml_test) 
{

	auto h = bricks_create_xtree_from_xml_file("test1.xml");

 	EXPECT_NE(h, 0);

	EXPECT_EQ(bricks_get_node_children_count(h, "/a/b/c"), 0);
	EXPECT_EQ(bricks_get_node_children_count(h,"/"), 1);
	EXPECT_EQ(bricks_get_node_children_count(h, "/a"), 3);
	EXPECT_EQ(bricks_get_node_children_count(h, "/a/b"), 1);
 	
}

TEST(xtree_case, xtree_parse_xml_test)
{

	auto h = bricks_create_xtree_from_xml_string("<a/>");

	EXPECT_NE(h, 0);

	EXPECT_EQ(bricks_get_node_children_count(h, "/a"), 0);
	EXPECT_EQ(bricks_get_node_children_count(h, "/"), 1);
	
}