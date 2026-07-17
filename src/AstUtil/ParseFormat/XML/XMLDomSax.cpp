///
/// @file      XMLDomSax.cpp
/// @brief     XML DOM SAX解析器实现
/// @author    axel
/// @date      2026-04-12
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

#include "XMLDomSax.hpp"
#include "AstUtil/GenericValue.hpp"
#include "AstUtil/Logger.hpp"


AST_NAMESPACE_BEGIN

XMLDomSax::XMLDomSax(XMLNode& root)
    : root_(root), nodeStack_() {
}

XMLDomSax::~XMLDomSax() {
}

void XMLDomSax::startDocument() {
}

void XMLDomSax::endDocument() {

}

void XMLDomSax::startElement(StringView name, const AttributeList& attributes) {
    XMLNode* elementNode = nullptr;
    if(nodeStack_.empty()){
        // 根节点
        elementNode = &root_;
        root_.clear();
        elementNode->setName(name);
        nodeStack_.push(&root_);
    }else{
        // 创建新元素节点
        std::unique_ptr<XMLNode> newNode(new XMLNode(EXMLNodeType::eElement, name));
        elementNode = newNode.get();
        nodeStack_.top()->addChild(std::move(newNode));
        nodeStack_.push(elementNode);
    }
       
    // 添加属性
    for (const auto& attr : attributes) {
        elementNode->addAttribute(std::string(attr.key()), GenericValue(attr.value()));
    }
}

void XMLDomSax::endElement(StringView name) {

    // 从栈中弹出当前节点
    if (!nodeStack_.empty() && StringView(nodeStack_.top()->getName()) == name) {
        nodeStack_.pop();
    }else{
        aError("unmatched end element: '%.*s'", name.size(), name.data());
    }
}

void XMLDomSax::characters(StringView text) {
    // 创建文本节点
    std::unique_ptr<XMLNode> textNode(new XMLNode(EXMLNodeType::eText, text));
    if (!nodeStack_.empty()) {
        nodeStack_.top()->addChild(std::move(textNode));
    }
}

void XMLDomSax::comment(StringView text) {
    // 创建注释节点
    std::unique_ptr<XMLNode> commentNode(new XMLNode(EXMLNodeType::eComment, text));
    if (!nodeStack_.empty()) {
        nodeStack_.top()->addChild(std::move(commentNode));
    }
}

void XMLDomSax::error(StringView msg) {
    // 简单处理错误，实际应用中可能需要更复杂的错误处理
    aError("XML error: '%.*s'", msg.size(), msg.data());
}


AST_NAMESPACE_END