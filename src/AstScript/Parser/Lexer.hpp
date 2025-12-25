///
/// @file      Lexer.hpp
/// @brief     词法分析器
/// @details   将输入的字符序列转换为令牌序列
/// @author    jinke18
/// @date      2025-12-19
/// @copyright 版权所有 (C) 2025-present, ast项目.
///
/// ast项目（https://github.com/space-ast/ast）
/// 本项目基于 Apache 2.0 开源许可证分发。
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
#include "AstScript/Scanner.hpp"
#include "AstUtil/StringView.hpp"


AST_NAMESPACE_BEGIN


/// @brief 词法分析器
class Lexer
{
public:
    /// @brief 令牌类型枚举
    enum ETokenType
    {
        // 特殊令牌
        eEndOfFile,  ///< 文件结束
        eError,      ///< 错误令牌
        
        // 字面量
        eNumber,     ///< 数字字面量
        eString,     ///< 字符串字面量
        eIdentifier, ///< 标识符
        eTrue,       ///< 布尔值true
        eFalse,      ///< 布尔值false
        eNullLiteral,///< 空值null
        
        // 运算符
        ePlus,       ///< +
        eMinus,      ///< -
        eStar,       ///< *
        eSlash,      ///< /
        ePercent,    ///< %
        eCaret,      ///< ^
        eTilde,      ///< ~ (按位取反)
        
        // 比较运算符
        eEqual,      ///< =
        eEqualEqual, ///< ==
        eBang,       ///< !
        eBangEqual,  ///< !=
        eLess,       ///< <
        eLessEqual,  ///< <=
        eLessLess,   ///< << (逻辑/算术左移)
        eGreater,    ///< >
        eGreaterEqual,///< >=
        eGreaterGreater,///< >> (算术右移)
        eGreaterGreaterGreater,///< >>> (逻辑右移)
        
        // 逻辑运算符
        eAmpersand,      ///< & (按位与/逻辑与)
        ePipe,           ///< | (按位或/逻辑或)
        eXor,            ///< ⊻ (按位异或)
        eAndAnd,         ///< && (逻辑与)
        eOrOr,           ///< || (逻辑或)
        
        // 括号
        eLeftParen,  ///< (
        eRightParen, ///< )
        eLeftBrace,  ///< {
        eRightBrace, ///< }

        eLeftBracket,  ///< [
        eRightBracket, ///< ]
        
        // 其他
        eComma,      ///< ,
        eDot,        ///< .
        eSemicolon,  ///< ;
        eColon,      ///< :
        eQuestion,   ///< ?

        eNewline,    ///< \n (换行符)
        
        // 代码块关键字（参照Julia语言）
        eBegin,      ///< begin
        eEnd,        ///< end
        
        // 条件语句关键字
        eIf,         ///< if
        eElseif,     ///< elseif
        eElse,       ///< else
        
        // 循环语句关键字
        eWhile,      ///< while
    };

public:
    Lexer(Scanner* scanner) 
        : scanner_(scanner)
        // , line_(1)
        , current_lexeme_() 
    {}
    
    /// @brief 获取下一个令牌
    ETokenType getNextToken();
    
    /// @brief 获取当前令牌的字符串视图
    StringView getCurrentLexeme() const { return current_lexeme_; }
    
    /// @brief 获取当前令牌的起始位置（始终返回空指针，因为不再存储实际位置）
    const char* getTokenStart() const { return nullptr; }
    
    /// @brief 获取当前令牌的结束位置（始终返回空指针，因为不再存储实际位置）
    const char* getTokenEnd() const { return nullptr; }
    
    /// @brief 获取当前行号
    // size_t getLine() const { return line_; }
    
    /// @brief 跳过空白字符
    void skipWhitespace();
    
private:
    /// @brief 检查当前字符是否是字母
    bool isAlpha(char c) const;
    
    /// @brief 检查当前字符是否是字母或数字
    bool isAlphaNumeric(char c) const;
    
    /// @brief 检查当前字符是否是数字
    bool isDigit(char c) const;
    
    /// @brief 检查当前字符是否是空白字符
    bool isWhitespace(char c) const;
    
    /// @brief 解析标识符或关键字
    int parseIdentifier();
    
    /// @brief 解析数字字面量
    Lexer::ETokenType scanNumber();
    
    /// @brief 解析字符串字面量
    Lexer::ETokenType scanString();
    
    /// @brief 匹配并消耗指定字符
    bool match(char expected);
    
    /// @brief 匹配并消耗指定字符串
    bool match(const char* str);
    
    /// @brief 消耗当前字符
    char advance();
    
    /// @brief 查看当前字符
    char peek() const;
    
    /// @brief 查看下一个字符
    char peekNext() const;
    
    /// @brief 检查是否到达结尾
    bool atEnd() const;
    
private:
    Scanner* scanner_;    ///< 扫描器指针
    // size_t line_;         ///< 当前行号
    std::string current_lexeme_; ///< 当前令牌的字符串内容
};

AST_NAMESPACE_END