#include "pch.h"
#include "bricks.h"

TEST(xtree_case, xtree_parse_xml_test) 
{

	auto h = bricks_create_xtree_from_xml_file("test1.xml");

 	EXPECT_NE(h, 0);

	EXPECT_EQ(bricks_get_node_children_count(h, "/a/b/c"), 0);
	EXPECT_EQ(bricks_get_node_children_count(h,"/"), 1);
	EXPECT_EQ(bricks_get_node_children_count(h, "/a"), 3);
	EXPECT_EQ(bricks_get_node_children_count(h, "/a/b"), 1);
 	
}