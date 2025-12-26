///
/// @file      Parser.cpp
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

#include "Parser.hpp"
#include "AstScript/Value.hpp"
#include "AstScript/ValInt.hpp"
#include "AstScript/ValDouble.hpp"
#include "AstScript/ValBool.hpp"
#include "AstScript/ValString.hpp"
#include "AstScript/ValNull.hpp"
#include "AstScript/ValQuantity.hpp"
#include "AstScript/ValRange.hpp"
#include "AstScript/OpBin.hpp"
#include "AstScript/OpAssign.hpp"
#include "AstScript/OpUnary.hpp"
#include "AstScript/Variable.hpp"
#include "AstScript/Symbol.hpp"
#include "AstScript/ExprBlock.hpp"
#include "AstScript/ExprCondition.hpp"
#include "AstScript/ExprIf.hpp"
#include "AstScript/ExprLoop.hpp"
#include "AstScript/ExprVector.hpp"
#include "AstScript/ExprCatHorizontal.hpp"
#include "AstScript/ExprCatVertical.hpp"
#include "Scanner.hpp"
#include "Lexer.hpp"
#include "AstUtil/QuantityParser.hpp"
#include "AstUtil/UnitParser.hpp"

#include <cstdlib>
#include <cctype>
#include <limits>

AST_NAMESPACE_BEGIN

/*
 * 表达式的文法定义
 * Expression → AssignExpr
 * AssignExpr → ConditionalExpr (AssignOp ConditionalExpr)?
 * ConditionalExpr → LogicalOrExpr ("?" Expression ":" Expression)?
 * LogicalOrExpr → LogicalAndExpr ("||" LogicalAndExpr)*
 * LogicalAndExpr → BitwiseOrExpr ("&&" BitwiseOrExpr)*
 * BitwiseOrExpr → BitwiseXorExpr ("|" BitwiseXorExpr)*
 * BitwiseXorExpr → BitwiseAndExpr ("^" BitwiseAndExpr)*
 * BitwiseAndExpr → EqualityExpr ("&" EqualityExpr)*
 * EqualityExpr → RelationalExpr (("==" | "!=") RelationalExpr)*
 * RelationalExpr → ShiftExpr (("<" | ">" | "<=" | ">=") ShiftExpr)*
 * ShiftExpr → AdditiveExpr (("<<" | ">>") AdditiveExpr)*
 * AdditiveExpr → MultiplicativeExpr (("+" | "-") MultiplicativeExpr)*
 * MultiplicativeExpr → UnaryExpr (("*" | "/" | "%") UnaryExpr)*
 * UnaryExpr → UnaryOp UnaryExpr | PrimaryExpr
 * PrimaryExpr → Literal | "(" Expression ")"
 */

// 类定义已移至 Parser.hpp 文件

// 实现Parser类的构造函数和成员函数
Parser::Parser(Lexer& lexer)
    : lexer_(lexer), currentTokenType_(lexer.getNextToken()) {}
    
/// @brief 获取当前令牌类型
int Parser::currentTokenType() const { return currentTokenType_; }
    
/// @brief 获取当前令牌的字符串视图
StringView Parser::currentLexeme() const { return lexer_.getCurrentLexeme(); }
    
/// @brief 获取当前行号
// size_t Parser::getLine() const { return lexer_.getLine(); }
    
/// @brief 前进到下一个令牌
void Parser::advance() { currentTokenType_ = lexer_.getNextToken(); }
    
/// @brief 匹配指定类型的令牌
bool Parser::match(int type)
{
    if (currentTokenType_ == type) {
        advance();
        return true;
    }
    return false;
}
    
/// @brief 检查当前令牌是否为指定类型
bool Parser::check(int type) const {
    return currentTokenType_ == type;
}
  
/// @brief 解析表达式
Expr* Parser::parseExpression()
{
    return parseAssignExpr();
}

/// @brief 解析语句序列（多个表达式，用分号或换行分隔）
// Expr* Parser::parseStatements()
// {
//     
// }

/// @brief 解析赋值表达式
Expr* Parser::parseAssignExpr()
{
    Expr* expr = parseConditionalExpr();
    if (!expr) {
        return nullptr;
    }
    
    if (match(Lexer::eEqual)) {
        Expr* right = parseAssignExpr();
        if (right) {
            return aNewOpAssign(EOpAssignType::eAssign, expr, right);
        }
        delete expr;
        return nullptr;
    } else if (match(Lexer::eEqualAmpersand)) {
        Expr* right = parseAssignExpr();
        if (right) {
            return aNewOpAssign(EOpAssignType::eBindAssign, expr, right);
        }
        delete expr;
        return nullptr;
    } else if (match(Lexer::eColonEqual)) {
        Expr* right = parseAssignExpr();
        if (right) {
            return aNewOpAssign(EOpAssignType::eDelayAssign, expr, right);
        }
        delete expr;
        return nullptr;
    }
    
    return expr;
}

/// @brief 解析条件表达式
Expr* Parser::parseConditionalExpr()
{
    // 解析条件部分
    Expr* condition = parseLogicalOrExpr();
    if (!condition) {
        return nullptr;
    }
    
    // 检查是否是条件表达式
    if (match(Lexer::eQuestion)) {
        // 解析then分支，使用parseShiftExpr()避免解析范围表达式，从而避免冒号被错误消耗
        Expr* thenExpr = parseShiftExpr();
        if (!thenExpr) {
            delete condition;
            return nullptr;
        }
        
        // 确保能匹配到冒号
        if (match(Lexer::eColon)) {
            // 解析else分支，同样使用parseShiftExpr()避免解析范围表达式
            Expr* elseExpr = parseShiftExpr();
            if (!elseExpr) {
                delete condition;
                delete thenExpr;
                return nullptr;
            }
            
            // 创建条件表达式对象
            return aNewExprCondition(condition, thenExpr, elseExpr);
        } else {
            delete condition;
            delete thenExpr;
            return nullptr;
        }
    }
    
    return condition;
}

/// @brief 解析逻辑或表达式
Expr* Parser::parseLogicalOrExpr()
{
    Expr* expr = parseLogicalAndExpr();
    
    while (match(Lexer::eOrOr)) {
        Expr* right = parseLogicalAndExpr();
        if (right) {
            expr = aNewOpBin(EOpBinType::eOr, expr, right);
        } else {
            delete expr;
            return nullptr;
        }
    }
    
    return expr;
}

/// @brief 解析逻辑与表达式
Expr* Parser::parseLogicalAndExpr()
{
    Expr* expr = parseBitwiseOrExpr();
    
    while (match(Lexer::eAndAnd)) {
        Expr* right = parseBitwiseOrExpr();
        if (right) {
            expr = aNewOpBin(EOpBinType::eAnd, expr, right);
        } else {
            delete expr;
            return nullptr;
        }
    }
    
    return expr;
}

/// @brief 解析位或表达式
Expr* Parser::parseBitwiseOrExpr()
{
    Expr* expr = parseBitwiseXorExpr();
    while (match(Lexer::ePipe)) {
        Expr* right = parseBitwiseXorExpr();
        if (right) {
            expr = aNewOpBin(EOpBinType::eBitOr, expr, right);
        } else {
            delete expr;
            return nullptr;
        }
    }
    
    return expr;
}

/// @brief 解析位异或表达式
Expr* Parser::parseBitwiseXorExpr()
{
    Expr* expr = parseBitwiseAndExpr();
    while (true) {
        if (match(Lexer::eXor)) {
            Expr* right = parseBitwiseAndExpr();
            if (right) {
                expr = aNewOpBin(EOpBinType::eBitXor, expr, right);
            } else {
                delete expr;
                return nullptr;
            }
        }else{
            break;
        }
    }
    
    return expr;
}

/// @brief 解析位与表达式
Expr* Parser::parseBitwiseAndExpr()
{
    Expr* expr = parseEqualityExpr();
    while (true) {
        if (match(Lexer::eAmpersand)) {
            Expr* right = parseEqualityExpr();
            if (right) {
                expr = aNewOpBin(EOpBinType::eBitAnd, expr, right);
            } else {
                delete expr;
                return nullptr;
            }
        }else{
            break;
        }
    }
    
    return expr;
}

/// @brief 解析相等性表达式
Expr* Parser::parseEqualityExpr()
{
    Expr* expr = parseRelationalExpr();
    
    while (true) {
        if (match(Lexer::eEqualEqual)) {
            Expr* right = parseRelationalExpr();
            if (right) {
                expr = aNewOpBin(EOpBinType::eEq, expr, right);
            } else {
                delete expr;
                return nullptr;
            }
        } else if (match(Lexer::eBangEqual)) {
            Expr* right = parseRelationalExpr();
            if (right) {
                expr = aNewOpBin(EOpBinType::eNe, expr, right);
            } else {
                delete expr;
                return nullptr;
            }
        } else {
            break;
        }
    }
    
    return expr;
}

/// @brief 解析关系表达式
Expr* Parser::parseRelationalExpr()
{
    // 解析起始表达式（使用位移表达式，因为关系表达式优先级高于范围表达式）
    Expr* expr = parseRangeExpr();
    
    while (true) {
        if (match(Lexer::eLess)) {
            Expr* right = parseRangeExpr();
            if (right) {
                expr = aNewOpBin(EOpBinType::eLt, expr, right);
            } else {
                delete expr;
                return nullptr;
            }
        } else if (match(Lexer::eLessEqual)) {
            Expr* right = parseRangeExpr();
            if (right) {
                expr = aNewOpBin(EOpBinType::eLe, expr, right);
            } else {
                delete expr;
                return nullptr;
            }
        } else if (match(Lexer::eGreater)) {
            Expr* right = parseRangeExpr();
            if (right) {
                expr = aNewOpBin(EOpBinType::eGt, expr, right);
            } else {
                delete expr;
                return nullptr;
            }
        } else if (match(Lexer::eGreaterEqual)) {
            Expr* right = parseRangeExpr();
            if (right) {
                expr = aNewOpBin(EOpBinType::eGe, expr, right);
            } else {
                delete expr;
                return nullptr;
            }
        } else {
            break;
        }
    }
        
    return expr;
}

/// @brief 解析范围表达式（如 1:10, 1:2:10）
Expr* Parser::parseRangeExpr()
{
    // 先解析起始值
    SharedPtr<Expr> startExpr = parseShiftExpr();
    if (!startExpr) {
        return nullptr;
    }
    
    // 检查是否有冒号
    if (match(Lexer::eColon)) {
        // 解析步长（可选）
        SharedPtr<Expr> secondExpr = parseShiftExpr();
        if (match(Lexer::eColon)) {
            SharedPtr<Expr> thirdExpr = parseShiftExpr();
            return aNewExprRange(startExpr, thirdExpr, secondExpr);
        }
        
        // 创建Range表达式
        return aNewExprRange(startExpr, secondExpr);
    }
    
    // 如果没有冒号，返回起始值表达式
    return startExpr.take();
}

/// @brief 解析移位表达式
Expr* Parser::parseShiftExpr()
{
    Expr* expr = parseAdditiveExpr();
    
    while (true) {
        if (match(Lexer::eLessLess)) {
            Expr* right = parseAdditiveExpr();
            if (right) {
                expr = aNewOpBin(EOpBinType::eBitLeftShift, expr, right);
            } else {
                delete expr;
                return nullptr;
            }
        } else if (match(Lexer::eGreaterGreater)) {
            Expr* right = parseAdditiveExpr();
            if (right) {
                expr = aNewOpBin(EOpBinType::eBitRightShift, expr, right);
            } else {
                delete expr;
                return nullptr;
            }
        } else if (match(Lexer::eGreaterGreaterGreater)) {
            Expr* right = parseAdditiveExpr();
            if (right) {
                expr = aNewOpBin(EOpBinType::eBitURightShift, expr, right);
            } else {
                delete expr;
                return nullptr;
            }
        } else {
            break;
        }
    }
    
    return expr;
}

/// @brief 解析加法表达式
Expr* Parser::parseAdditiveExpr()
{
    Expr* expr = parseMultiplicativeExpr();
    
    while (true) {
        if (match(Lexer::ePlus)) {
            Expr* right = parseMultiplicativeExpr();
            if (right) {
                expr = aNewOpBin(EOpBinType::eAdd, expr, right);
            } else {
                delete expr;
                return nullptr;
            }
        } else if (match(Lexer::eMinus)) {
            Expr* right = parseMultiplicativeExpr();
            if (right) {
                expr = aNewOpBin(EOpBinType::eSub, expr, right);
            } else {
                delete expr;
                return nullptr;
            }
        } else {
            break;
        }
    }
    
    return expr;
}

/// @brief 解析乘法表达式
Expr* Parser::parseMultiplicativeExpr()
{
    Expr* expr = parseExponentiationExpr();
    
    while (true) {
        if (match(Lexer::eStar)) {
            Expr* right = parseExponentiationExpr();
            if (right) {
                expr = aNewOpBin(EOpBinType::eMul, expr, right);
            } else {
                delete expr;
                return nullptr;
            }
        } else if (match(Lexer::eSlash)) {
            Expr* right = parseExponentiationExpr();
            if (right) {
                expr = aNewOpBin(EOpBinType::eDiv, expr, right);
            } else {
                delete expr;
                return nullptr;
            }
        } else if (match(Lexer::ePercent)) {
            Expr* right = parseExponentiationExpr();
            if (right) {
                expr = aNewOpBin(EOpBinType::eMod, expr, right);
            } else {
                delete expr;
                return nullptr;
            }
        } else {
            break;
        }
    }
    
    return expr;
}

/// @brief 解析幂指数表达式（右结合）
Expr* Parser::parseExponentiationExpr()
{
    Expr* expr = parseUnaryExpr();
    
    if (match(Lexer::eCaret)) {
        Expr* right = parseExponentiationExpr();
        if (right) {
            return aNewOpBin(EOpBinType::ePow, expr, right);
        } else {
            delete expr;
            return nullptr;
        }
    }
    
    return expr;
}

/// @brief 解析一元表达式
Expr* Parser::parseUnaryExpr()
{
    if (match(Lexer::eMinus)) {
        Expr* expr = parseUnaryExpr();
        if (expr) {
            return aNewOpUnary(EOpUnaryType::eNeg, expr);
        }
        return nullptr;
    } else if (match(Lexer::eBang)) {
        Expr* expr = parseUnaryExpr();
        if (expr) {
            return aNewOpUnary(EOpUnaryType::eNot, expr);
        }
        return nullptr;
    } else if (match(Lexer::eTilde)) {
        Expr* expr = parseUnaryExpr();
        if (expr) {
            return aNewOpUnary(EOpUnaryType::eBitNot, expr);
        }
        return nullptr;
    }
    
    return parsePrimaryExpr();
}

/// @brief 解析带begin/end的代码块
Expr* Parser::parseBeginEndBlock()
{
    if (!match(Lexer::eBegin)) {
        return nullptr;
    }

    // 检查begin和end之间是否有表达式
    int tokenType = currentTokenType();
    if (tokenType == Lexer::eEnd) {
        // 空代码块，直接返回一个空的ExprBlock
        advance();
        return (new ExprBlock());
    }
    
    // 解析语句序列
    Expr* statements = parseStatementSequence();
    
    // 匹配end关键字
    if (!match(Lexer::eEnd)) {
        delete statements;
        return nullptr;
    }
    
    return statements;
}


static bool isEndOfBlock(int tokenType)
{
    return 
       tokenType == Lexer::eEndOfFile 
    || tokenType == Lexer::eEnd
    || tokenType == Lexer::eElse 
    || tokenType == Lexer::eElseif
    ;
}

/// @brief 解析语句序列（多个表达式，用分号或换行分隔）
Expr* Parser::parseStatementSequence()
{
    // 跳过开头的所有连续分隔符（换行符和分号）
    while (currentTokenType() == Lexer::eNewline || currentTokenType() == Lexer::eSemicolon) {
        advance();
    }
    
    // 如果是标识block末尾的关键字，返回一个空的ExprBlock
    if (isEndOfBlock(currentTokenType())) {
        // 空的语句序列，返回一个空的ExprBlock
        return new ExprBlock();
    }
    
    // 解析第一个表达式
    Expr* firstExpr = parseExpression();
    if (!firstExpr) {
        return nullptr;
    }
    
    // 检查是否到达文件末尾或end关键字
    int tokenType = currentTokenType();
    if (isEndOfBlock(tokenType)) {
        // 如果只有一个表达式，直接返回
        return firstExpr;
    }
    
    // 创建一个代码块
    SharedPtr<ExprBlock> block = new ExprBlock();
    block->addExpr(firstExpr);
    
    // 解析剩余的所有表达式，直到文件末尾或end关键字
    while (currentTokenType() != Lexer::eEndOfFile) {
        tokenType = currentTokenType();
        
        // 检查是否有换行符或分号作为分隔符
        if (tokenType == Lexer::eNewline || tokenType == Lexer::eSemicolon) {
            // 跳过所有连续的分隔符
            while (currentTokenType() == Lexer::eNewline || currentTokenType() == Lexer::eSemicolon) {
                advance();
            }
        } else if (isEndOfBlock(tokenType)) {
            break;
        } else {
            return nullptr;
        }
        
        // 检查是否已经到达文件末尾或end关键字
        if (isEndOfBlock(currentTokenType())) {
            break;
        }
        
        // 解析下一个表达式
        Expr* expr = parseExpression();
        if (expr) {
            block->addExpr(expr);
        } else {
            return nullptr;
        }
    }
    
    return block.take();
}

/// @brief 解析代码块表达式
Expr* Parser::parseBlockExpr()
{
    // 先检查是否是带begin/end的代码块
    // 如果已经匹配到begin关键字，就不应该再尝试解析语句序列
    if (check(Lexer::eBegin)) {
        return parseBeginEndBlock();
    }
    
    // 否则解析语句序列
    return parseStatementSequence();
}

/// @brief 解析if条件语句
Expr* Parser::parseIfStatement() {
    if (!match(Lexer::eIf)) {
        return nullptr;
    }
    
    // 解析条件表达式
    Expr* condition = parseExpression();
    if (!condition) {
        return nullptr;
    }
    
    // 解析then分支（代码块或单个表达式）
    Expr* thenBlock = parseBlockExpr();
    if (!thenBlock) {
        delete condition;
        return nullptr;
    }
    
    // 创建if表达式
    SharedPtr<ExprIf> ifExpr = new ExprIf(condition, thenBlock);
    
    // 解析所有elseif分支
    while (match(Lexer::eElseif)) {
        // 解析elseif条件
        Expr* elseifCondition = parseExpression();
        if (!elseifCondition) {
            // delete ifExpr;
            return nullptr;
        }
        
        // 解析elseif代码块
        Expr* elseifBlock = parseBlockExpr();
        if (!elseifBlock) {
            // delete ifExpr;
            delete elseifCondition;
            return nullptr;
        }
        
        ifExpr->addElseif(elseifCondition, elseifBlock);
    }
    
    // 解析else分支（可选）
    if (match(Lexer::eElse)) {
        Expr* elseBlock = parseBlockExpr();
        if (!elseBlock) {
            delete ifExpr;
            return nullptr;
        }
        ifExpr->setElse(elseBlock);
    }

    if (!match(Lexer::eEnd)) {
        aError("expect end");
        return nullptr;
    }
    
    return ifExpr.take();
}

/// @brief 解析while循环语句
Expr* Parser::parseWhileLoop()
{
    if (!match(Lexer::eWhile)) {
        return nullptr;
    }
    
    // 解析条件表达式
    Expr* condition = parseExpression();
    if (!condition) {
        return nullptr;
    }
    
    // 解析循环体（代码块或单个表达式）
    Expr* body = parseBlockExpr();
    if (!body) {
        delete condition;
        return nullptr;
    }
    
    // 匹配end关键字
    if (!match(Lexer::eEnd)) {
        delete condition;
        delete body;
        aError("Expected 'end' after while loop body");
        return nullptr;
    }
    
    // 创建并返回While循环表达式
    return new ExprWhile(condition, body);
}

/// @brief 解析for循环语句（范围风格）
Expr* Parser::parseForRangeLoop()
{
    if (!match(Lexer::eFor)) {
        return nullptr;
    }
    
    // 解析循环变量
    Expr* variable = parsePrimaryExpr();
    if (!variable) {
        return nullptr;
    }
    
    // 匹配in关键字
    if (!match(Lexer::eIn) && !match(Lexer::eEqual)) {
        delete variable;
        return nullptr;
    }
    
    // 解析范围表达式
    Expr* range = parseExpression();
    if (!range) {
        delete variable;
        return nullptr;
    }
    
    // 解析循环体（代码块或单个表达式）
    Expr* body = parseBlockExpr();
    if (!body) {
        delete variable;
        delete range;
        return nullptr;
    }
    
    // 匹配end关键字
    if (!match(Lexer::eEnd)) {
        delete variable;
        delete range;
        delete body;
        aError("Expected 'end' after for loop body");
        return nullptr;
    }
    
    // 创建并返回ForRange循环表达式
    return new ExprForRange(variable, range, body);
}

/// @brief 解析基本表达式
Expr* Parser::parsePrimaryExpr()
{
    // 处理if语句
    if(check(Lexer::eIf)){
        return parseIfStatement();
    }else if(check(Lexer::eWhile)){
        return parseWhileLoop();
    }else if(check(Lexer::eFor)){
        return parseForRangeLoop();
    }else if(check(Lexer::eBegin)){
        return parseBeginEndBlock();
    }
    
    // 处理括号表达式
    if (match(Lexer::eLeftParen)) {
        Expr* expr = parseExpression();
        if (match(Lexer::eRightParen)) {
            return expr;
        }
        delete expr;
        return nullptr;
    }
    
    // 处理数组和拼接语法 [1,2,3], [a,b,c], [1 2 3], [a b c], [1;2;3], [a;b;c]
    if (match(Lexer::eLeftBracket)) {
        // 解析第一个表达式
        Expr* firstExpr = parseExpression();
        if (!firstExpr) {
            // 空数组 []
            if (match(Lexer::eRightBracket)) {
                return new ExprVector();
            }
            return nullptr;
        }
        
        // 检查分隔符类型
        if (match(Lexer::eComma)) {
            // 逗号分隔的数组 [1,2,3], [a,b,c]
            ExprVector* vector = new ExprVector();
            vector->push_back(firstExpr);
            
            // 解析剩余的表达式
            do {
                Expr* expr = parseExpression();
                if (!expr) {
                    delete vector;
                    return nullptr;
                }
                vector->push_back(expr);
            } while (match(Lexer::eComma));
            
            if (match(Lexer::eRightBracket)) {
                return vector;
            }
            delete vector;
            return nullptr;
        } else if (match(Lexer::eSemicolon)) {
            // 分号分隔的垂直拼接 [1;2;3], [a;b;c]
            ExprCatVertical* vertical = new ExprCatVertical();
            vertical->push_back(firstExpr);
            
            // 解析剩余的表达式
            do {
                Expr* expr = parseExpression();
                if (!expr) {
                    delete vertical;
                    return nullptr;
                }
                vertical->push_back(expr);
            } while (match(Lexer::eSemicolon));
            
            if (match(Lexer::eRightBracket)) {
                return vertical;
            }
            delete vertical;
            return nullptr;
        } else {
            // 空格分隔的水平拼接 [1 2 3], [a b c]
            ExprCatHorizontal* horizontal = new ExprCatHorizontal();
            horizontal->push_back(firstExpr);
            
            // 解析剩余的表达式
            while (check(Lexer::eRightBracket) == false) {
                Expr* expr = parseExpression();
                if (!expr) {
                    delete horizontal;
                    return nullptr;
                }
                horizontal->push_back(expr);
            }
            
            if (match(Lexer::eRightBracket)) {
                return horizontal;
            }
            delete horizontal;
            return nullptr;
        }
    }
    
    // 处理嵌套的begin/end代码块
    Expr* block = parseBeginEndBlock();
    if (block) {
        return block;
    }
    
    if (currentTokenType() == Lexer::eNumber) {
        std::string numStr = currentLexeme().to_string();
        advance();
        
        // 检查是否后面紧跟单位（支持 1.2[km/s]、12 [km]、1.2 [m] 格式）
        if (match(Lexer::eLeftBracket)) {
            // 解析单位部分
            std::string unitStr;
            
            // 跳过可能的空白符
            while (!match(Lexer::eRightBracket)) {
                unitStr += currentLexeme().to_string();
                advance();
            }
            
            // 组合成完整的数量字符串并解析
            Unit unit;
            err_t err = aUnitParse(unitStr, unit);
            if (err != eNoError) {
                aError("Invalid unit: %s", unitStr.c_str());
                return nullptr;
            }
            double value = std::strtod(numStr.data(), nullptr);
            Quantity quantity {value, unit};
            return aNewValueQuantity(quantity);
        } else {
            // 普通数字解析
            // @todo: 下面的逻辑有待优化
            // 检查是否为浮点数
            if (numStr.find_first_of("eE.") != std::string::npos && numStr[1] != 'x') 
            {
                double value = std::strtod(numStr.data(), nullptr);
                return new ValDouble(value);
            } else {
                long long value = 0;
                if(numStr.size() >= 2 && numStr[0] == '0' && (numStr[1] == 'x' || numStr[1] == 'b' || numStr[1] == 'o')){
                    // 检查是否为十六进制数字
                    if ((numStr[1] == 'x')) {
                        // 使用 strtoll 的十六进制解析功能
                        value = std::strtoll(numStr.data(), nullptr, 16);
                    } 
                    // 检查是否为二进制数字
                    else if ((numStr[1] == 'b')) {
                        // 使用stoll的二进制解析功能
                        value = std::strtoll(numStr.data() + 2, nullptr, 2);
                    }
                    // 检查是否为八进制数字，参照Julia语法
                    else if ((numStr[1] == 'o')) {
                        // 使用stoll的八进制解析功能
                        value = std::strtoll(numStr.data() + 2, nullptr, 8);
                    }
                }
                // 十进制数字
                else
                {
                    value = std::strtoll(numStr.data(), nullptr, 10);
                }
                
                // 检查是否超出int范围
                if (value < std::numeric_limits<int>::min() || value > std::numeric_limits<int>::max()) {
                    // 超出int范围
                    return nullptr;
                }
                return new ValInt(static_cast<int>(value));
            }
        }
    }
    
    if (currentTokenType() == Lexer::eString) {
        StringView str = currentLexeme();
        auto retval = new ValString(String(str.data(), str.size()));
        advance();
        return retval;
    }
    
    if (match(Lexer::eTrue)) {
        return new ValBool(true);
    }
    
    if (match(Lexer::eFalse)) {
        return new ValBool(false);
    }
    
    if (match(Lexer::eNullLiteral)) {
        return new ValNull();
    }
    
    if (currentTokenType() == Lexer::eIdentifier) {
        auto var = aNewSymbol(currentLexeme());
        advance();
        return var;
    }
    
    return nullptr;
}

/// @brief 解析表达式
/// @param script 脚本文本
/// @return Expr* 解析得到的表达式对象，解析失败返回nullptr
Expr* Parser::parseExpr(StringView script)
{
    StringScanner scanner(script);
    Lexer lexer(&scanner);
    Parser context(lexer);
    
    // 使用parseStatements方法处理单个或多个表达式
    Expr* expr = context.parseBlockExpr();
    
    return expr;
}

AST_NAMESPACE_END