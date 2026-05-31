///
/// @file      XMLNode.cpp
/// @brief     
/// @details   
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

#include "XMLNode.hpp"
#include "XMLParser.hpp"
#include "AstUtil/Logger.hpp"
#include "AstUtil/IO.hpp"
#include <cstdio>
#include <cassert>

AST_NAMESPACE_BEGIN

XMLNode::XMLNode(EXMLNodeType kind, StringView nameOrText)
    : kind_(kind)
    , nameOrText_(nameOrText)
{
}

XMLNode::XMLNode(StringView nameOrText)
    : kind_(EXMLNodeType::eElement) 
    , nameOrText_(nameOrText)
{
}



XMLNode::~XMLNode()
{
    clearChildren();
}

void XMLNode::setName(StringView name)
{
    assert(kind_ == EXMLNodeType::eElement);
    nameOrText_ = std::string(name);
}


void XMLNode::setText(StringView text)
{
    assert(kind_ == EXMLNodeType::eText || kind_ == EXMLNodeType::eComment);
    nameOrText_ = std::string(text);
}

errc_t XMLNode::load(StringView filepath)
{
    XMLParser parser(filepath);
    return parser.parse(*this);
}

errc_t XMLNode::save(StringView filepath)
{
    FILE* file = fopen(filepath.data(), "w");
    if (!file) {
        return errc_t(1);
    }

    // 写入 XML 声明
    posix::fprintf(file, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");

    // 递归生成 XML 内容
    if (!generateXML(file, 0)) {
        fclose(file);
        return errc_t(1);
    }

    fclose(file);
    return eNoError;
}

bool XMLNode::generateXML(FILE* file, int indent) const
{
    std::string indentStr(indent, ' ');

    switch (kind_) {
    case EXMLNodeType::eElement:
        // 写入开始标签
        posix::fprintf(file, "%s<%s", indentStr.c_str(), getName().c_str());
        
        // 写入属性
        for (std::map<std::string, GenericValue>::const_iterator it = attributes_.begin(); it != attributes_.end(); ++it) {
            const std::string& attrName = it->first;
            const GenericValue& attrValue = it->second;
            posix::fprintf(file, " %s=\"%s\"", attrName.c_str(), escapeXML(attrValue.value()).c_str());
        }
        
        if (children_.empty()) {
            // 空元素
            posix::fprintf(file, "/>\n");
        } else {
            posix::fprintf(file, ">\n");
            
            
            // 写入子节点
            for (std::vector<HXMLNode>::const_iterator it = children_.begin(); it != children_.end(); ++it) {
                if (!(*it)->generateXML(file, indent + 2)) {
                    return false;
                }
            }
            
            // 写入结束标签
            posix::fprintf(file, "%s</%s>\n", indentStr.c_str(), getName().c_str());
        }
        break;
        
    case EXMLNodeType::eText:
        {
            std::string escapedText = escapeXML(getText());
            posix::fprintf(file, "%s%s\n", indentStr.c_str(), escapedText.c_str());
        }
        break;
        
    case EXMLNodeType::eComment:
        posix::fprintf(file, "%s<!-- %s -->\n", indentStr.c_str(), getText().c_str());
        break;
        
    case EXMLNodeType::eCDATA:
        posix::fprintf(file, "%s<![CDATA[%s]]>\n", indentStr.c_str(), getText().c_str());
        break;
        
    case EXMLNodeType::eDocType:
        posix::fprintf(file, "%s<!DOCTYPE %s>\n", indentStr.c_str(), getText().c_str());
        break;
    }

    return true;
}

std::string XMLNode::escapeXML(const std::string& str) const
{
    std::string result;
    result.reserve(str.size());
    
    for (char c : str) {
        switch (c) {
        case '<':
            result += "&lt;";
            break;
        case '>':
            result += "&gt;";
            break;
        case '&':
            result += "&amp;";
            break;
        case '"':
            result += "&quot;";
            break;
        case '\'':
            result += "&apos;";
            break;
        default:
            result += c;
            break;
        }
    }
    
    return result;
}

ValueView XMLNode::getAttribute(const std::string & name)
{
    auto it = attributes_.find(name);
    if(it == attributes_.end()){
        return {};
    }
    return it->second;
}

void XMLNode::clearChildren()
{
    children_.clear();
}

void XMLNode::clearAttributes()
{
    attributes_.clear();
}

void XMLNode::clear()
{
    kind_ = EXMLNodeType::eElement;
    clearChildren();
    clearAttributes();
    nameOrText_.clear();
}



void XMLNode::addChild(HXMLNode&& child)
{
    children_.push_back(std::move(child));
}


void XMLNode::addText(StringView text)
{
    addChild(std::unique_ptr<XMLNode>(new XMLNode(EXMLNodeType::eText, text)));
}

void XMLNode::addComment(StringView text)
{
    addChild(std::unique_ptr<XMLNode>(new XMLNode(EXMLNodeType::eComment, text)));
}






void XMLNode::addAttribute(const std::string& name, const GenericValue& value)
{
    attributes_[name] = value;
}

AST_NAMESPACE_END

