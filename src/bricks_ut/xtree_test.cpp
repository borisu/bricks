#include "pch.h"
#include "xtree.h"

using namespace bricks;

TEST(xtree_case, xtree_load_xml_test_and_children) 
{

	xtree_t* xt = create_xtree_from_xml(
		"<configuration>"
		" <property name = \"bool.value\"   value=\"true\">"
		"    <subproperty>AQIDBAU=</subproperty>"
		" </property>"
		" <property name = \"int.value\"    value=\"5\" />"
		" <property name = \"double.value\" value=\"5.0\" />"
		" <property name = \"str.value\"    value=\"Mother do you think\" />"
		"</configuration>");

	
	EXPECT_EQ(xt->get_node_children_count("/configuration").value(), 4);
	EXPECT_EQ(xt->get_node_children_count("/configuration/property").value(), 1);
	EXPECT_EQ(xt->get_node_children_count("/configuration/property/subproperty").value(), 0);

	EXPECT_EQ(xt->get_node_name(xp_t("/configuration/property",0)).value(), "subproperty");

	

	EXPECT_EQ(get<double>(xt->get_property_value(xp_t("/configuration", 2), "value").value()), 5.0);

	const vector_t* buf = xt->get_node_value("/configuration/property/subproperty").value();

	EXPECT_EQ(*buf, vector_t({ 1,2,3,4,5 }));

	xt->release();

 	
}

TEST(xtree_case, xtree_create_empty_xml)
{
	auto xt = create_xtree();

	EXPECT_EQ("<?xml version=\"1.0\" encoding=\"UTF-8\"?>", serialize_xtree_to_xml(xt));

	EXPECT_EQ(false, xt->get_node("").has_value());
	EXPECT_EQ(false, xt->get_node(xp_t("",1)).has_value());
	EXPECT_EQ(false, xt->get_node("/").has_value());
	EXPECT_EQ(false, xt->get_node("ufo").has_value());
	EXPECT_EQ(false, xt->set_property_value("/","p",1));
	EXPECT_EQ(false, xt->set_property_value("", "p", 1));
	EXPECT_EQ(false, xt->set_node_name("", "n"));
	EXPECT_EQ(false, xt->set_node_value("", "abc",3));

	xt->release();
}

TEST(xtree_case, xtree_create_1_level_replicate_root)
{
	auto xt = create_xtree();

	auto n1 = xt->add_node("root");

	auto n11 = xt->add_node("root");

	EXPECT_EQ(n1, n11);

	EXPECT_EQ("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
		"<root/>\n",
		serialize_xtree_to_xml(xt));

	EXPECT_EQ(false, xt->add_node("root",true).has_value());

	EXPECT_EQ("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
		"<root/>\n",
		serialize_xtree_to_xml(xt));

	xt->release();

}

TEST(xtree_case, xtree_create_2_level_with_clone)
{
	auto xt = create_xtree();

	xt->add_node("root");
	xt->add_node("root/child", true);
	xt->add_node("root/child", true);

	EXPECT_EQ("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
		"<root>\n"
		"    <child/>\n"
		"    <child/>\n"
		"</root>\n",
		serialize_xtree_to_xml(xt));

	auto dst = create_xtree();
	clone_xtree(xt, xp_t(xt->get_root().value()), dst, xp_t(""));

	EXPECT_EQ("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
		"<root>\n"
		"    <child/>\n"
		"    <child/>\n"
		"</root>\n",
		serialize_xtree_to_xml(dst));

	xt->release();
	dst->release();

}



TEST(xtree_case, xtree_create_1_level_remove_subtree)
{
	auto xt = create_xtree();

	char abc[] = { 1,2,3,4,5 };

	xt->set_node_value("root", abc, 5);


	EXPECT_EQ("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
		"<root>AQIDBAU=</root>\n",
		serialize_xtree_to_xml(xt));

	xt->remove_subtree("root");


	EXPECT_EQ("<?xml version=\"1.0\" encoding=\"UTF-8\"?>",
		serialize_xtree_to_xml(xt));

	xt->release();

}


TEST(xtree_case, xtree_create_1_level_value_test)
{
	auto xt = create_xtree();

	char abc[] = {1,2,3,4,5};

	xt->set_node_value("root", abc , 5);

	EXPECT_EQ("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
		      "<root>AQIDBAU=</root>\n",
		serialize_xtree_to_xml(xt));

	xt->remove_node_value("root");


	EXPECT_EQ("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
		      "<root/>\n",
		serialize_xtree_to_xml(xt));

	xt->release();

}
TEST(xtree_case, xtree_ip_parse)
{
	brick_uptr<xtree_t> s_xt(
		create_xtree_from_xml(
			"<configuration>"
			" <property name = \"bootstrap.servers\" value=\"127.0.0.1:29092\"/>"
			" <property name = \"group.id\" value=\"mygroup\"/>"
			" <property name = \"auto.offset.reset\" value=\"latest\"/>"
			"</configuration>"
		));

	auto value = s_xt->get_property_value_as_string(xp_t("/configuration", 0), "value").value();
	EXPECT_EQ("127.0.0.1:29092", value);


}

TEST(xtree_case, xtree_enforce_type)
{
	xtree_t* xt = create_xtree_from_xml(
		"<configuration>"
		" <property int_value=\"%i:5\"  longlong_value=\"%L:5\" double_value=\"%d:5\" string_value=\"%s:5\" bool_value1=\"%b:true\" bool_value2=\"%b:valse\"/>"
		"</configuration>");

	EXPECT_EQ(get<int>(xt->get_property_value("/configuration/property", "int_value").value()), 5);
	EXPECT_EQ(get<long long>(xt->get_property_value("/configuration/property", "longlong_value").value()), 5);
	EXPECT_EQ(get<double>(xt->get_property_value("/configuration/property", "double_value").value()), 5.0);
	EXPECT_EQ(get<string>(xt->get_property_value("/configuration/property", "string_value").value()), "5");
	EXPECT_EQ(get<bool>(xt->get_property_value("/configuration/property", "bool_value1").value()), true);
	EXPECT_EQ(get<bool>(xt->get_property_value("/configuration/property", "bool_value2").value()), false);

	xt->release();

}

TEST(xtree_case, xtree_create_1_level)
{
	auto xt = create_xtree();

	auto n1 = xt->add_node("root");

	EXPECT_EQ("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
		      "<root/>\n",
		serialize_xtree_to_xml(xt));

	EXPECT_EQ(false, xt->add_node("").has_value());
	EXPECT_EQ(false, xt->add_node("",true).has_value());
	EXPECT_EQ(false, xt->get_node("").has_value());
	

	auto n11 = xt->get_node("/root").value();

	EXPECT_EQ(n1,n11);

	auto n111 = xt->get_node(xp_t(n11, "/")).value();

	EXPECT_EQ(n1, n111);

	auto n1111 = xt->get_node(xp_t(n11, "")).value();

	EXPECT_EQ(n1, n1111);

	xt->set_property_value("/root", "double", 2.2);
	xt->set_property_value("/root", "double1", 1.0);
	xt->set_property_value("/root", "int", 1);
	xt->set_property_value("/root", "bool", false);
	xt->set_property_value("/root", "string", string("the wall"));

	EXPECT_EQ("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
		      "<root double=\"%d:2.2\" double1=\"%d:1.0\" int=\"%i:1\" bool=\"%b:false\" string=\"%s:the wall\"/>\n",
	   serialize_xtree_to_xml(xt));

	// do it twice
	xt->set_property_value("/root", "double", 2.2);
	xt->set_property_value("/root", "double1", 1.0);
	xt->set_property_value("/root", "int", 1);
	xt->set_property_value("/root", "bool", false);
	xt->set_property_value("/root", "string", string("the wall"));

	xt->add_node("root");

	EXPECT_EQ("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
		"<root double=\"%d:2.2\" double1=\"%d:1.0\" int=\"%i:1\" bool=\"%b:false\" string=\"%s:the wall\"/>\n",
		serialize_xtree_to_xml(xt));

	xt->remove_property("root","double1");
	xt->remove_property("root", "double1");

	xt->set_property_value("/root", "longlong", (long long)25);

	EXPECT_EQ("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
		"<root double=\"%d:2.2\" int=\"%i:1\" bool=\"%b:false\" string=\"%s:the wall\" longlong=\"%L:25\"/>\n",
		serialize_xtree_to_xml(xt));

	xt->release();
}



