///
/// @file      XMLNode.hpp
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

#pragma once

#include "AstGlobal.h"
#include "AstUtil/GenericValue.hpp"
#include "AstUtil/ValueView.hpp"
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <cstdio>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

enum class EXMLNodeType
{
    eElement,       ///< 元素节点
    eText,          ///< 文本节点
    eComment,       ///< 注释节点
    eCDATA,         ///< CDATA节点
    eDocType,       ///< 文档类型节点
};

class XMLNode;
using HXMLNode = std::unique_ptr<XMLNode>;

/// @brief XML节点
class AST_UTIL_API XMLNode
{
public:
    XMLNode() = default;

    XMLNode(EXMLNodeType kind, StringView nameOrText);

    explicit XMLNode(StringView nameOrText);

    ~XMLNode();

    /// @brief 从XML解析器加载节点
    /// @param filepath 文件路径
    errc_t load(StringView filepath);

    /// @brief 保存节点到XML文件
    /// @param filepath 文件路径
    errc_t save(StringView filepath);

    
    /// @brief 获取节点类型
    /// @return 节点类型
    /// @brief 获取节点类型
    /// @return 节点类型
    EXMLNodeType getKind() const { return kind_; }

    /// @brief 获取节点名称
    /// @return 节点名称
    const std::string& getName() const { return nameOrText_; }

    /// @brief 设置节点名称
    /// @param name 节点名称
    void setName(StringView name);

    /// @brief 获取节点文本
    /// @return 节点文本
    const std::string& getText() const { return nameOrText_; }

    /// @brief 设置节点文本
    /// @param text 节点文本
    void setText(StringView text);

    /// @brief 添加子节点
    /// @param child 子节点
    void addChild(HXMLNode&& child);

    /// @brief 添加文本节点
    /// @param text 文本内容
    void addText(StringView text);

    /// @brief 添加注释节点
    /// @param text 注释内容
    void addComment(StringView text);

    /// @brief 获取子节点
    /// @details 从 startIndex 开始查找子节点，返回第一个匹配的子节点。
    ///         如果未找到匹配的子节点，返回 nullptr。
    /// @param name 子节点名称
    /// @param startIndex 子节点索引
    /// @return 子节点
    // XMLNode* getChild(StringView name, size_t startIndex = 0) const;

    /// @brief 获取子节点列表
    /// @return 子节点列表
    const std::vector<HXMLNode>& getChildren() const { return children_; }


    /// @brief 添加属性
    /// @param name 属性名称
    /// @param value 属性值
    void addAttribute(const std::string& name, const GenericValue& value);


    /// @brief 获取属性列表
    /// @return 属性列表
    const std::map<std::string, GenericValue>& getAttributes() const { return attributes_; }


    /// @brief 获取属性值
    /// @param name 属性名称
    /// @return 属性值
    ValueView getAttribute(const std::string& name);


    /// @brief 清除子节点
    /// @details 递归清除所有子节点，包括子节点的子节点。
    void clearChildren();

    /// @brief 清除属性
    /// @details 清除节点的所有属性。
    void clearAttributes();

    /// @brief 清除节点内容
    /// @details 清除节点的所有子节点和属性。
    void clear();
private:
    A_DISABLE_COPY(XMLNode);
    
    /// @brief 生成 XML 内容
    /// @param file 文件指针
    /// @param indent 缩进级别
    /// @return 是否生成成功
    bool generateXML(FILE* file, int indent) const;

    /// @brief 转义 XML 特殊字符
    /// @param str 原始字符串
    /// @return 转义后的字符串
    std::string escapeXML(const std::string& str) const;

private:
    EXMLNodeType kind_{EXMLNodeType::eElement};      ///< 节点类型
    std::string nameOrText_{};                       ///< 节点名称或文本
    std::vector<HXMLNode> children_;                 ///< 子节点列表
    std::map<std::string, GenericValue> attributes_; ///< 属性列表
};

/*! @} */

AST_NAMESPACE_END