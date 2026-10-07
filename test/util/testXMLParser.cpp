///
/// @file      testXMLParser.cpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2026-04-11
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
/// 本软件基于 Apache 2.0 开源许可证分发。
/// 您可在遵守许可证条款的前提下使用、修改和分发本软件。
/// 许可证全文请见：
/// 
///    http://www.apache.org/licenses/LICENSE-2.0
/// 
/// 重要须知：
/// 软件按"现有状态"提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。

#include "ast/XMLParser.hpp"
#include "ast/XMLSax.hpp"
#include "ast/XMLDocument.hpp"
#include "ast/AstTestMacro.h"
#include <string>
#include <vector>
#include <memory>

AST_USING_NAMESPACE

class TestXMLSax : public XMLSax {
public:
    void startDocument() override {
        printf("<!-- start document -->\n");
    }
    
    void endDocument() override {
        printf("<!-- end document -->\n");
    }
    
    void startElement(StringView name, const AttributeList& attributes) override {
        printf("%s", std::string(indent, ' ').c_str());
        printf("<%s", std::string(name).c_str());
        for (const auto& attr : attributes) {
            printf(" %s=\"%s\"", std::string(attr.key()).c_str(), attr.value().toString().c_str());
        }
        printf(">\n");
        indent++;
    }
    
    void endElement(StringView name) override {
        indent--;
        printf("%s", std::string(indent, ' ').c_str());
        printf("</%s>\n", std::string(name).c_str());
    }
    
    void characters(StringView text) override {
        printf("%s", std::string(indent, ' ').c_str());
        auto iter = text.find_first_of("<>");
        if (iter != StringView::npos) {
            printf("<![CDATA[");
        }
        printf("%s", std::string(text).c_str());
        if (iter != StringView::npos) {
            printf("]]>\n");
        }else{
            printf("\n");
        }
    }
    
    void comment(StringView text) override {
        printf("%s", std::string(indent, ' ').c_str());
        printf("<!-- %s -->\n", std::string(text).c_str());
    }
private:
    int indent = 0;
};

TEST(XMLParserTest, Basic) {
    // 创建一个简单的XML字符串
    const char* xml = R"(
        <?xml version="1.0" encoding="UTF-8"?>
        <root>
            <child id="1" name="test">
                Hello World
            </child>
            <!-- This is a comment -->
            <child id="2" name="test2">
                <grandchild/>
            </child>
        </root>
    )";
    
    // 写入临时文件
    FILE* file = fopen("test.xml", "w");
    fprintf(file, "%s", xml);
    fclose(file);
    
    // 创建解析器和SAX处理器
    XMLParser parser("test.xml");
    TestXMLSax sax;
    
    // 解析XML
    errc_t err = parser.parse(sax);
    EXPECT_EQ(err, eNoError);
    
    
    
    // 清理临时文件
    remove("test.xml");
}

TEST(XMLParserTest, CDATA) {
    // 创建包含CDATA的XML字符串
    const char* xml = R"(
        <root>
            <data><![CDATA[<test>Hello</test>]]></data>
        </root>
    )";
    
    // 写入临时文件
    FILE* file = fopen("test_cdata.xml", "w");
    fprintf(file, "%s", xml);
    fclose(file);
    
    // 创建解析器和SAX处理器
    XMLParser parser("test_cdata.xml");
    TestXMLSax sax;
    
    // 解析XML
    errc_t err = parser.parse(sax);
    EXPECT_EQ(err, eNoError);
    
    
    // 清理临时文件
    remove("test_cdata.xml");
}

TEST(XMLParserTest, Save) {
    // 创建一个XML节点
    XMLNode root("root");
    
    // 添加属性
    root.addAttribute("version", "1.0");
    root.addAttribute("encoding", "UTF-8");
    
    // 添加子节点
    std::unique_ptr<XMLNode> child1(new XMLNode("child"));
    child1->addAttribute("id", "1");
    child1->addAttribute("name", "test");
    child1->addText("Hello World");
    root.addChild(std::move(child1));
    
    // 添加注释节点
    root.addComment("This is a comment");

    // 添加带孙子节点的子节点
    std::unique_ptr<XMLNode> child2(new XMLNode("child"));
    child2->addAttribute("id", "2");
    child2->addAttribute("name", "test2");
    
    std::unique_ptr<XMLNode> grandchild(new XMLNode("grandchild"));
    child2->addChild(std::move(grandchild));
    
    root.addChild(std::move(child2));
    
    // 保存到文件
    errc_t err = root.save("test_save.xml");
    EXPECT_EQ(err, eNoError);
    
    // 打印生成的 XML 文件内容
    printf("Generated XML:\n");
    FILE* file = fopen("test_save.xml", "r");
    if (file) {
        char buffer[1024];
        while (fgets(buffer, sizeof(buffer), file)) {
            printf("%s", buffer);
        }
        fclose(file);
    }
    
    // 读取并验证
    XMLNode loadedRoot;
    err = loadedRoot.load("test_save.xml");
    EXPECT_EQ(err, eNoError);
    
    // 验证根节点属性
    EXPECT_EQ(loadedRoot.getName(), "root");
    
    EXPECT_EQ(loadedRoot.getName(), "root");
        
    // 验证 <root> 元素的子节点数量（应该是 3，即一个注释节点和两个子 <child> 元素）
    EXPECT_EQ(loadedRoot.getChildren().size(), 3);
    
    // 清理临时文件
    remove("test_save.xml");
}

TEST(XMLParserTest, XMLDocument) {
    // 创建XML文档
    XMLDocument doc;
    
    // 创建根节点
    XMLNode* root = doc.getRoot();
    root->setName("document");
    EXPECT_NE(root, nullptr);
    
    // 添加属性
    root->addAttribute("version", "1.0");
    root->addAttribute("encoding", "UTF-8");
    
    // 添加子节点
    std::unique_ptr<XMLNode> child(new XMLNode("element"));
    child->addAttribute("id", "1");
    child->addText("Test content");
    root->addChild(std::move(child));
    
    // 保存到文件
    errc_t err = doc.save("test_document.xml");
    EXPECT_EQ(err, eNoError);
    
    // 打印生成的 XML 文件内容
    printf("Generated XML Document:\n");
    FILE* file = fopen("test_document.xml", "r");
    if (file) {
        char buffer[1024];
        while (fgets(buffer, sizeof(buffer), file)) {
            printf("%s", buffer);
        }
        fclose(file);
    }
    
    // 加载文档
    XMLDocument loadedDoc;
    err = loadedDoc.load("test_document.xml");
    EXPECT_EQ(err, eNoError);
    
    // 验证根节点
    // 获取 <document> 元素
    XMLNode* loadedRoot = loadedDoc.getRoot();
    EXPECT_NE(loadedRoot, nullptr);
    XMLNode* documentElement = loadedRoot;
    EXPECT_EQ(documentElement->getName(), "document");
    
    // 验证 <document> 元素的子节点数量（应该是 1，即 <element> 元素）
    EXPECT_EQ(documentElement->getChildren().size(), 1);
    
    // 清理临时文件
    remove("test_document.xml");
}


TEST(XMLParserTest, LoadSTK)
{
    if(aIsCI())
        GTEST_SKIP();
    std::vector<std::string> stkFiles = aTestGetConfigStringVector("STK_PROPAGATOR_FILES");
    for(const auto& file : stkFiles)
    {
        printf("Loading %s\n", file.c_str());
        XMLParser parser(file);
        TestXMLSax sax;
        errc_t err = parser.parse(sax);
        EXPECT_EQ(err, eNoError);
    }
}

GTEST_MAIN()