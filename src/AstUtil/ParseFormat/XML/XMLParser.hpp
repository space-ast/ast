///
/// @file      XMLParser.hpp
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
#include "AstUtil/BaseParser.hpp"
#include "AstUtil/BKVItemView.hpp"
#include "AstUtil/XMLNode.hpp"
#include "AstUtil/XMLSax.hpp"
#include <string>
#include <vector>
#include <memory>
#include <deque>


AST_NAMESPACE_BEGIN


/*!
    @addtogroup 
    @{
*/

class XMLNode;
class XMLSax;


/// @brief 轻量级的SAX解析器
/// @details 用于解析XML文档，支持流式解析，无需加载整个文档到内存
class AST_UTIL_API XMLParser : public BaseParser 
{
public:
    enum EToken
    {
        eError = -1,               ///< 错误
        eStartDocument,            ///< 文档开始
        eEndDocument,              ///< 文档结束
        eStartElement,             ///< 元素开始
        eEndElement,               ///< 元素结束
        eCharacters,               ///< 文本内容
        eComment,                  ///< 注释
        eDocTypeDecl,              ///< DOCTYPE声明
        eProcessingInstruction,    ///< 处理指令
        // eUnknown,                  ///< 未知元素
    };

    XMLParser();
    explicit XMLParser(StringView filepath);
    ~XMLParser();

    /// @brief 解析XML文档
    errc_t parse(XMLSax& sax);
    /// @brief 解析XML文档到节点
    /// @param node 解析结果节点
    errc_t parse(XMLNode& node);
    /// @brief 获取下一个解析令牌
    /// @return 下一个解析令牌
    EToken getNext();
    /// @brief 获取元素名称
    /// @return 元素名称
    StringView getName() const{return nameOrText_;}
    /// @brief 获取文本
    /// @return 文本
    StringView getText() const{return nameOrText_;}
    /// @brief 获取注释
    /// @return 注释
    StringView getComment() const{return nameOrText_;}
    /// @brief 获取属性列表
    /// @return 属性列表
    const XMLSax::AttributeList& getAttributes() const{return attributes_;}
protected:
    void setName(StringView name){nameOrText_ = name;}
    void setText(StringView text){nameOrText_ = text;}
    void setComment(StringView comment){nameOrText_ = comment;}
private:
    /// @brief 追加缓冲区
    /// @param size 追加大小
    errc_t appendBuffer(size_t size);

    /// @brief 追加缓冲区，默认大小
    errc_t appendBuffer();


    /// @brief 预览下一个字符（不消耗）
    int peek();
    
    /// @brief 获取下一个字符（消耗）
    int get();

    /// @brief 确保缓冲区至少有n个字符
    StringView ensure(size_t n);

    /// @brief 跳转到当前解析位置
    /// @details 确保解析位置与文件指针位置一致，归还缓冲区剩余空间
    /// @param pos 解析位置
    void seekFileToCurrent();

private: // XML 片段解析函数

    EToken parseStartElement();
    EToken parseEndElement();
    EToken parseText();
    EToken parseComment();
    EToken parseCDATA();
    EToken parsePI();
    EToken parseDocTypeDecl();
    
    /// @brief 解析直到指定字符或序列
    StringView parseUntil(char stopChar);
    /// @brief 解析直到指定字符序列
    StringView parseUntil(StringView stopSeq);
    /// @brief 前进n个字符
    /// @param n 前进字符数
    void advance(int n);
    /// @brief 跳过空白字符
    void skipWhitespace();
private:
    size_t pos_{0};                        ///< 当前解析位置
    std::vector<char> buffer_{};           ///< 缓冲区
    XMLSax::AttributeList attributes_{};   ///< 属性列表
    StringView nameOrText_{};              ///< 元素名称或文本内容
    bool selfClosingTag_{false};           ///< 自闭合标签标志
    std::string unescapedText_{};          ///< 未转义的文本内容
};


/*! @} */

AST_NAMESPACE_END