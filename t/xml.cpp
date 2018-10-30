#include "gtest/gtest.h"

#include "reproweb/xml/document.h"

using namespace reproweb;


class BasicTest : public ::testing::Test {
 protected:

  virtual void SetUp() 
  {}

  virtual void TearDown() 
  {}
};

const char* testXML = "<root xmlns='http://blabla' xmlns:x='http://xyz'>\r\n"
"<x:element>\r\n"
"<name>littlemole</name>\r\n"
"<email>littlemole@oha7.org</email>\r\n"
"<url>n/a</url>\r\n"
"</x:element>\r\n"
"</root>\r\n";

void dump_node(xml::Node* n)
{
	std::cout << n->nodeName() << std::endl;
	std::cout << ((xml::Element*)n)->innerXml() << std::endl;
}


TEST_F(BasicTest, SimpleXml)
{
	xml::Document doc;
	doc.parse(testXML);

	xml::Node* root = doc.documentElement()->childNodes()->item(0);
	dump_node(root);

	xml::Node* elem = root->childNodes()->getChildByName("element");
	dump_node(elem);

	std::cout << elem->prefix() << ":" << elem->namespaceURI() << std::endl;
	std::cout << elem->getPrefixFromNS("http://xyz") << ":" << elem->getNSfromPrefix("x") << std::endl;

	xml::Node* name = elem->childNodes()->getChildByName("name");
	dump_node(name);

	std::cout << name->prefix() << ":" << name->namespaceURI() << std::endl;
	std::cout << name->getPrefixFromNS("http://blabla") << ":" << name->getNSfromPrefix("") << std::endl;

	xml::Node* email = elem->childNodes()->getChildByName("email");
	dump_node(email);

	xml::Node* url = elem->childNodes()->getChildByName("url");
	dump_node(url);

	std::cout << ((xml::Element*)doc.documentElement()->childNodes()->item(0))->outerXml() << std::endl;
}


int main(int argc, char **argv) 
{
	::testing::InitGoogleTest(&argc, argv);
    int r = RUN_ALL_TESTS();

    return r;
}
