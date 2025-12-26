///
/// @file      Parser.hpp
/// @brief     ~
/// @details   ~
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
#include "AstScript/Expr.hpp"
#include "Lexer.hpp"

AST_NAMESPACE_BEGIN

/// @brief 脚本语法解析器
class AST_SCRIPT_API Parser
{
public:
    Parser(Lexer& lexer);
    
    /// @brief 获取当前令牌类型
    int currentTokenType() const;
    
    /// @brief 获取当前令牌的字符串视图
    StringView currentLexeme() const;
    
    /// @brief 获取当前行号
    size_t getLine() const;
    
    /// @brief 前进到下一个令牌
    void advance();
    
    /// @brief 匹配指定类型的令牌
    bool match(int type);
    
    /// @brief 检查当前令牌是否为指定类型
    bool check(int type) const;
    
    /// @brief 解析表达式
    Expr* parseExpression();
    
    /// @brief 解析语句序列（多个表达式，用分号或换行分隔）
    Expr* parseStatements();
    
    /// @brief 解析赋值表达式
    Expr* parseAssignExpr();
    
    /// @brief 解析条件表达式
    Expr* parseConditionalExpr();
    
    /// @brief 解析逻辑或表达式
    Expr* parseLogicalOrExpr();
    
    /// @brief 解析逻辑与表达式
    Expr* parseLogicalAndExpr();
    
    /// @brief 解析按位或表达式
    Expr* parseBitwiseOrExpr();
    
    /// @brief 解析按位异或表达式
    Expr* parseBitwiseXorExpr();
    
    /// @brief 解析按位与表达式
    Expr* parseBitwiseAndExpr();
    
    /// @brief 解析相等性表达式
    Expr* parseEqualityExpr();
    
    /// @brief 解析关系表达式
    Expr* parseRelationalExpr();
    
    /// @brief 解析范围表达式（如 1:10, 1:2:10）
    Expr* parseRangeExpr();
    
    /// @brief 从给定的起始表达式解析范围表达式
    Expr* parseRangeExprFrom(Expr* startExpr);
    
    /// @brief 解析位移表达式
    Expr* parseShiftExpr();
    
    /// @brief 解析加法表达式
    Expr* parseAdditiveExpr();
    
    /// @brief 解析乘法表达式
    Expr* parseMultiplicativeExpr();
    
    /// @brief 解析幂指数表达式
    Expr* parseExponentiationExpr();
    
    /// @brief 解析一元表达式
    Expr* parseUnaryExpr();
    
    /// @brief 解析带begin/end的代码块
    Expr* parseBeginEndBlock();
    
    /// @brief 解析语句序列（多个表达式，用分号或换行分隔）
    Expr* parseStatementSequence();
    
    /// @brief 解析代码块表达式
    Expr* parseBlockExpr();
    
    /// @brief 解析基本表达式
    Expr* parsePrimaryExpr();
    
    /// @brief 解析if条件语句
    Expr* parseIfStatement();
    
    /// @brief 解析while循环语句
    Expr* parseWhileLoop();
    
    /// @brief 解析表达式
    /// @param script 脚本文本
    /// @return Expr* 解析得到的表达式对象，解析失败返回nullptr
    static Expr* parseExpr(StringView script);
private:
    Lexer& lexer_;
    Lexer::ETokenType currentTokenType_;
};

AST_NAMESPACE_END