/// @file      UnitParser.cpp
/// @brief     单位解析器实现，使用递归下降方法
/// @details   解析复合单位表达式，支持乘除运算（左结合）和指数运算（右结合）
/// @author    jinke18
/// @date      2025-12-15
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

#include "UnitParser.hpp"
#include "UnitManager.hpp"
#include "AstUtil/ParseFormat.hpp"
#include "Unit.hpp"
#include <stack>
#include <vector>
#include <string>

AST_NAMESPACE_BEGIN


/*
 * 单位的文法定义
 * Expression → Term
 * Term → Factor (('*' | '/') Factor)*      // 左结合
 * Factor → Primary ('^' Factor)?           // 右结合
 * Primary → '(' Expression ')' | BasicUnit
 * BasicUnit → [a-zA-Z][a-zA-Z0-9]*
 */

namespace{
/// @brief 解析器上下文
class ParserContext
{
public:
    ParserContext(const char* begin, const char* end)
        : current_(begin), begin_(begin), end_(end) {}
    
    /// @brief 跳过空白字符
    void skipWhitespace()
    {
        while (current_ < end_ && (*current_ == ' ' || *current_ == '\t'))
        {
            ++current_;
        }
    }
    
    /// @brief 检查是否到达结尾
    bool atEnd() const { return current_ >= end_; }
    
    /// @brief 查看当前字符（不移动指针）
    char peek() const
    {
        return atEnd() ? '\0' : *current_;
    }
    
    /// @brief 读取并消耗当前字符
    char consume()
    {
        return atEnd() ? '\0' : *current_++;
    }
    
    /// @brief 检查当前字符是否为指定字符
    bool check(char c)
    {
        skipWhitespace();
        return !atEnd() && *current_ == c;
    }
    
    /// @brief 匹配并跳过指定字符
    bool match(char c)
    {
        skipWhitespace();
        if (!atEnd() && *current_ == c)
        {
            ++current_;
            return true;
        }
        return false;
    }

    /// @brief 匹配并跳过指定字符串
    bool match(const char* str)
    {
        skipWhitespace();
        size_t len = strlen(str);
        if (strncmp(current_, str, len) == 0)
        {
            current_ += len;
            return true;
        }
        return false;
    }
    
    /// @brief 获取当前位置
    const char* position() const { return current_; }
    /// @brief 获取表达式开始位置
    const char* begin() const { return begin_; }
    /// @brief 获取表达式结束位置
    const char* end() const { return end_; }
private:
    const char* current_;
    const char* begin_;
    const char* end_;
};
}

/// @brief 检查字符是否为字母或数字
static bool isAlphanumeric(char c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9');
}



/// @brief 解析基本单位字符串
static err_t parseBasicUnit(StringView unitName, Unit& unit)
{
    // 检查是否已经存在单位
    auto unit_found = aUnitGet(unitName);
    if (unit_found)
    {
        unit = unit_found->clone();  // 返回单位的副本
        return eNoError;
    }
    return eErrorParse;
}

// 前向声明
static err_t parseTerm(ParserContext& ctx, Unit& unit);

/// @brief 解析基本单位或括号表达式
/// @param[in] ctx 解析器上下文
/// @param[out] unit 解析出的单位
/// @return 错误码
static err_t parsePrimary(ParserContext& ctx, Unit& unit)
{
    ctx.skipWhitespace();
    
    if (ctx.atEnd())
    {
        return eErrorParse;
    }
    
    // 处理括号表达式
    if (ctx.match('('))
    {
        // 递归解析括号内的表达式
        err_t err = parseTerm(ctx, unit);
        if (err != eNoError)
        {
            return err;
        }
        
        // 期望匹配右括号
        if (!ctx.match(')'))
        {
            return eErrorParse; // 缺少右括号
        }
        return eNoError;
    }
    
    // 处理基本单位
    const char* start = ctx.position();
    
    // 读取单位名称（字母部分）
    while (!ctx.atEnd() && isAlphanumeric(ctx.peek()))
    {
        ctx.consume();
    }
    
    if (ctx.position() == start)
    {
        return eErrorParse; // 没有读取到任何字符
    }
    
    StringView unitName(start, ctx.position() - start);
    return parseBasicUnit(unitName, unit);
}

/// @brief 解析整数组成的表达式
/// @param[in] ctx 解析器上下文
/// @param[out] value 解析出的整数
/// @return 错误码
static err_t parseExpression(ParserContext& ctx, int &value)
{
    if (ctx.match('('))
    {
        // 递归解析括号内的表达式
        err_t err = parseExpression(ctx, value);
        if (err != eNoError)
        {
            return err;
        }
        
        // 期望匹配右括号
        if (!ctx.match(')'))
        {
            return eErrorParse; // 缺少右括号
        }
    }
    else
    {
        const char* start = ctx.position();
        // 读取整数（可能带负号）
        while (!ctx.atEnd() && ((ctx.peek() >= '0' && ctx.peek() <= '9') || ctx.peek() == '-'))
        {
            ctx.consume();
        }
        
        if (ctx.position() == start)
        {
            return eErrorParse; // 整数部分为空
        }
        
        StringView expStr(start, ctx.position() - start);
        err_t err = aParseInt(expStr, value);
        if(err != eNoError)
        {
            return err;
        }
        
    }
    if (ctx.match('^'))
    {
        int exponent = 0;
        err_t err = parseExpression(ctx, exponent);
        if (err != eNoError)
        {
            return err;
        }
        value = std::pow(value, exponent);
        return eNoError;
    }
    return eNoError;
}



/// @brief 解析因子（包括指数运算，右结合）
/// 文法：Factor → Primary ^ Factor | Primary
/// 注意：指数是右结合的，a^b^c 解析为 a^(b^c)
static err_t parseFactor(ParserContext& ctx, Unit& unit)
{
    // 解析基本单位或括号表达式
    err_t err = parsePrimary(ctx, unit);
    if (err != eNoError)
    {
        return err;
    }
    
    // 检查是否有指数运算符（右结合）
    if (ctx.match('^'))
    {
        // 解析指数（必须是整数）
        int exponent = 0;
        err = parseExpression(ctx, exponent);
        if (err != eNoError)
        {
            return err;
        }
        
        // 应用指数
        unit = unit.pow(exponent);
    }else if(ctx.peek() == (aText("⁰")[0]) || ctx.peek() == (aText("¹")[0]) || ctx.peek() == (aText("⁻")[0]))
    {
        int sign = 1;
        if (ctx.match(aText("⁻")))
        {
            sign = -1;
        }

        static const char *number_to_superscript[] =
        {
            aText("⁰"), aText("¹"), aText("²"), aText("³"), aText("⁴"), 
            aText("⁵"), aText("⁶"), aText("⁷"), aText("⁸"), aText("⁹")
        };
        for (int i = 0; i < 10; ++i)
        {
            if (ctx.match(number_to_superscript[i]))
            {
                unit = unit.pow(sign * i);
                break;
            }
        }
    }
    
    return eNoError;
}

/// @brief 解析项（乘除法，左结合）
/// @details 文法：Term → Factor (('*' | '/') Factor)*
/// 注意：乘除法是左结合的，a*b/c*d 解析为 (((a*b)/c)*d)
static err_t parseTerm(ParserContext& ctx, Unit& unit)
{
    // 解析第一个因子
    err_t err = parseFactor(ctx, unit);
    if (err != eNoError)
    {
        return err;
    }
    
    // 左结合处理乘除法
    while (true)
    {
        ctx.skipWhitespace();
        
        if (ctx.atEnd())
        {
            break;
        }
        
        char op = ctx.peek();
        if (ctx.match("*") || ctx.match("/") || ctx.match(aText("·")))
        {
            // 解析右侧的因子
            Unit right;
            err = parseFactor(ctx, right);
            if (err != eNoError)
            {
                return err;
            }
            
            // 应用运算符（左结合）
            if (op == '/')
            {
                unit = unit / right;
            }
            else    // '*' or "·"
            {
                unit = unit * right;
            }
        }
        else if (op == ')')
        {
            // 遇到右括号，停止当前层级的解析
            break;
        }
        else // if (op == ' ' || op == '\t')
        {
            // 空格视为隐式乘法（如果后面跟着因子）
            ctx.skipWhitespace();
            
            // 检查空格后是否是有效字符（字母或数字表示隐式乘法）
            if (!ctx.atEnd() && (isAlphanumeric(ctx.peek()) || ctx.peek() == '('))
            {
                // 隐式乘法
                Unit right;
                err = parseFactor(ctx, right);
                if (err != eNoError)
                {
                    return err;
                }
                unit = unit * right;
            }
            else {
                //其他字符，结束当前项
                break;
            }
        }
    }
    
    return eNoError;
}

/// @brief 解析完整表达式（顶层解析函数）
/// @param[in] ctx 解析器上下文
/// @param[out] unit 解析出的单位
/// @return 错误码
static err_t parseExpression(ParserContext& ctx, Unit& unit)
{
    return parseTerm(ctx, unit);
}

/// @brief 解析复合单位字符串（使用递归下降方法）
/// @param[in] unitName 复合单位字符串
/// @param[out] unit 解析后的单位
/// @return err_t 错误码
static err_t parseCompoundUnit(StringView unitName, Unit& unit)
{
    ParserContext ctx(unitName.begin(), unitName.end());
    
    // 解析表达式
    err_t err = parseExpression(ctx, unit);
    if (err != eNoError)
    {
        return err;
    }
    
    // 确保消耗了所有输入
    ctx.skipWhitespace();
    if (!ctx.atEnd())
    {
        return eErrorParse; // 有未解析的字符
    }
    
    return eNoError;
}

/// @brief 解析单位字符串
err_t aUnitParse(StringView str, Unit& unit)
{
    // 检查输入是否为空
    if (str.empty())
    {
        unit = Unit::NaN();
        return eErrorParse;
    }
    
    // 尝试直接解析为基本单位
    err_t err = parseBasicUnit(str, unit);
    if (err == eNoError)
    {
        return err;
    }
    
    // 尝试解析为复合单位
    err = parseCompoundUnit(str, unit);
    if(err)
    {
        unit = Unit::NaN();
    }
    return err;
}

/// @brief 解析单位字符串
Unit aUnitParse(StringView str)
{
    Unit unit;
    err_t err = aUnitParse(str, unit);
    if (err != eNoError)
    {
        return Unit::NaN();
    }
    return unit;
}

AST_NAMESPACE_END