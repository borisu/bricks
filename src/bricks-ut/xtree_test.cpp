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

TEST(xtree_case, xtree_create_empty_xml)
{
	auto xt = create_xtree();

	EXPECT_EQ("<?xml version=\"1.0\" encoding=\"UTF-8\"?>", serialize_xtree_to_xml(xt));

}

TEST(xtree_case, xtree_create_1_level_replicate_root)
{
	auto xt = create_xtree();

	auto n1 = xt->add_node("root");

	auto n11 = xt->add_node("root");

	EXPECT_EQ(n1, n11);

	EXPECT_EQ(false, xt->add_node("root",true).has_value());

	EXPECT_EQ("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
		"<root/>\n",
		serialize_xtree_to_xml(xt));

}

TEST(xtree_case, xtree_create_1_level)
{
	auto xt = create_xtree();

	auto n1 = xt->add_node("root");

	EXPECT_EQ("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
		      "<root/>\n",
		serialize_xtree_to_xml(xt));

	auto n11 = xt->get_node("/root").value();

	EXPECT_EQ(n1,n11);

	auto n111 = xt->get_node(n11, "/").value();

	EXPECT_EQ(n1, n111);

	auto n1111 = xt->get_node(n11, "").value();

	EXPECT_EQ(n1, n1111);

	xt->set_property_value("/root", "double", 2.2);
	xt->set_property_value("/root", "double1", 1.0);
	xt->set_property_value("/root", "int", 1);
	xt->set_property_value("/root", "bool", false);
	xt->set_property_value("/root", "string", string("the wall"));

	EXPECT_EQ("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
		      "<root double=\"2.2\" double1=\"1.0\" int=\"1\" bool=\"false\" string=\"the wall\"/>\n",
	   serialize_xtree_to_xml(xt));

	// do it twice
	xt->set_property_value("/root", "double", 2.2);
	xt->set_property_value("/root", "double1", 1.0);
	xt->set_property_value("/root", "int", 1);
	xt->set_property_value("/root", "bool", false);
	xt->set_property_value("/root", "string", string("the wall"));

	xt->add_node("root");

	EXPECT_EQ("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
		"<root double=\"2.2\" double1=\"1.0\" int=\"1\" bool=\"false\" string=\"the wall\"/>\n",
		serialize_xtree_to_xml(xt));

	xt->remove_property("root","double1");
	xt->remove_property("root", "double1");

	EXPECT_EQ("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
		"<root double=\"2.2\" int=\"1\" bool=\"false\" string=\"the wall\"/>\n",
		serialize_xtree_to_xml(xt));

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
