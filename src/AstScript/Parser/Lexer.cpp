///
/// @file      Lexer.cpp
/// @brief     词法分析器实现
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

#include "Lexer.hpp"

#include <cctype>
#include <cstring>
#include <unordered_map>

AST_NAMESPACE_BEGIN

/// @brief 关键字映射表
static const std::unordered_map<StringView, Lexer::ETokenType> keywords = {
    {"true", Lexer::eTrue},
    {"false", Lexer::eFalse},
    {"null", Lexer::eNullLiteral},
    {"and", Lexer::eAndAnd},
    {"or", Lexer::eOrOr}
};

/// @brief 获取下一个令牌
Lexer::ETokenType Lexer::getNextToken()
{
    skipWhitespace();
    
    if (atEnd()) {
        current_lexeme_.clear();
        return Lexer::eEndOfFile;
    }
    
    char c = advance();
    
    if (isAlpha(c)) {
        current_lexeme_.clear();
        current_lexeme_ += c;
        
        while (isAlphaNumeric(peek())) {
            current_lexeme_ += advance();
        }
        
        StringView lexeme(current_lexeme_);
        auto it = keywords.find(lexeme);
        if (it != keywords.end()) {
            return it->second;
        }
        
        return Lexer::eIdentifier;
    }
    
    if (isDigit(c)) {
        current_lexeme_.clear();
        current_lexeme_ += c;
        return scanNumber();
    }
    
    current_lexeme_.clear();
    current_lexeme_ += c;
    
    switch (c) {
        // 单字符令牌
        case '(':
            return Lexer::eLeftParen;
        case ')':
            return Lexer::eRightParen;
        case '{':
            return Lexer::eLeftBrace;
        case '}':
            return Lexer::eRightBrace;
        case ',':
            return Lexer::eComma;
        case '.':
            return Lexer::eDot;
        case ';':
            return Lexer::eSemicolon;
        case ':':
            return Lexer::eColon;
        case '?':
            return Lexer::eQuestion;
        
        // 可能的双字符令牌
        case '!':
            if (match('=')) {
                current_lexeme_ += '=';
                return Lexer::eBangEqual;
            }
            return Lexer::eBang;
        case '=':
            if (match('=')) {
                current_lexeme_ += '=';
                return Lexer::eEqualEqual;
            }
            return Lexer::eEqual;
        case '<':
            if (match('=')) {
                current_lexeme_ += '=';
                return Lexer::eLessEqual;
            }
            return Lexer::eLess;
        case '>':
            if (match('=')) {
                current_lexeme_ += '=';
                return Lexer::eGreaterEqual;
            }
            return Lexer::eGreater;
        case '+':
            return Lexer::ePlus;
        case '-':
            return Lexer::eMinus;
        case '*':
            return Lexer::eStar;
        case '/':
            return Lexer::eSlash;
        case '%':
            return Lexer::ePercent;
        
        // 字符串字面量
        case '"':
			// case '\'': @todo: 支持单引号字符串
            current_lexeme_.clear(); // 引号不包含在字符串内容中
            return scanString();
        case '&':
            if (match('&')) {
                current_lexeme_ += '&';
                return Lexer::eAndAnd;
            }
            return Lexer::eAmpersand;
        case '|':
            if (match('|')) {
                current_lexeme_ += '|';
                return Lexer::eOrOr;
            }
            return Lexer::ePipe;   
        case '^':
            return Lexer::eCaret;
        default:
            return Lexer::eError;
    }
}

/// @brief 跳过空白字符
void Lexer::skipWhitespace()
{
    while (!atEnd()) {
        char c = peek();
        switch (c) {
            case ' ':
            case '\t':
            case '\r':
                advance();
                break;
            case '\n':
                line_++;
                advance();
                break;
            case '/':
                if (peekNext() == '/') {
                    // 单行注释
                    while (peek() != '\n' && !atEnd()) {
                        advance();
                    }
                } else {
                    return;
                }
                break;
            default:
                return;
        }
    }
}

/// @brief 检查当前字符是否是字母
bool Lexer::isAlpha(char c) const
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

/// @brief 检查当前字符是否是字母或数字
bool Lexer::isAlphaNumeric(char c) const
{
    return isAlpha(c) || isDigit(c);
}

/// @brief 检查当前字符是否是数字
bool Lexer::isDigit(char c) const
{
    return c >= '0' && c <= '9';
}

/// @brief 检查当前字符是否是空白字符
bool Lexer::isWhitespace(char c) const
{
    return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

/// @brief 解析标识符或关键字
int Lexer::parseIdentifier()
{
    // current_lexeme_已经在getNextToken中初始化
    
    while (isAlphaNumeric(peek())) {
        current_lexeme_ += advance();
    }
    
    StringView lexeme(current_lexeme_);
    
    auto it = keywords.find(lexeme);
    if (it != keywords.end()) {
        return it->second;
    }
    
    return Lexer::eIdentifier;
}

/// @brief 解析数字字面量
Lexer::ETokenType Lexer::scanNumber()
{
    // current_lexeme_已经在getNextToken中初始化
    
    // 整数部分
    while (isDigit(peek())) {
        current_lexeme_ += advance();
    }
    
    // 小数部分
    if (peek() == '.' && isDigit(peekNext())) {
        current_lexeme_ += advance(); // 跳过小数点
        
        while (isDigit(peek())) {
            current_lexeme_ += advance();
        }
    }
    
    // 指数部分
    if ((peek() == 'e' || peek() == 'E')) {
        current_lexeme_ += advance(); // 跳过e或E
        
        if (peek() == '+' || peek() == '-') {
            current_lexeme_ += advance(); // 跳过符号
        }
        
        while (isDigit(peek())) {
            current_lexeme_ += advance();
        }
    }
    
    return Lexer::eNumber;
}

/// @brief 解析字符串字面量
Lexer::ETokenType Lexer::scanString()
{
    // current_lexeme_已经在getNextToken中清空
    
    while (peek() != '"' && !atEnd()) {
        if (peek() == '\n') {
            line_++;
        }
        current_lexeme_ += advance();
    }
    
    if (atEnd()) {
        return Lexer::eError;
    }
    
    advance(); // 跳过右引号
    
    return Lexer::eString;
}

/// @brief 匹配并消耗指定字符
bool Lexer::match(char expected)
{
    if (atEnd() || peek() != expected) {
        return false;
    }
    
    advance();
    return true;
}

/// @brief 消耗当前字符
char Lexer::advance()
{
    return scanner_->consume();
}

/// @brief 查看当前字符
char Lexer::peek() const
{
    return scanner_->peek();
}

/// @brief 查看下一个字符
char Lexer::peekNext() const
{
    return scanner_->peekNext();
}

/// @brief 检查是否到达结尾
bool Lexer::atEnd() const
{
    return scanner_->atEnd();
}

AST_NAMESPACE_END