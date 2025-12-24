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
#include "AstScript/OpBin.hpp"
#include "AstScript/OpAssign.hpp"
#include "AstScript/OpUnary.hpp"
#include "AstScript/Variable.hpp"
#include "AstScript/Symbol.hpp"
#include "AstScript/ExprBlock.hpp"
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
    
    if (match(Lexer::eEqual)) {
        Expr* right = parseAssignExpr();
        if (right) {
            return aNewOpAssign(EOpAssignType::eAssign, expr, right);
        }
        delete expr;
        return nullptr;
    }
    
    return expr;
}

/// @brief 解析条件表达式
Expr* Parser::parseConditionalExpr()
{
    Expr* expr = parseLogicalOrExpr();
    
    if (match(Lexer::eQuestion)) {
        Expr* thenExpr = parseExpression();
        if (!thenExpr) {
            delete expr;
            return nullptr;
        }
        
        if (!match(Lexer::eColon)) {
            delete expr;
            delete thenExpr;
            return nullptr;
        }
        
        Expr* elseExpr = parseConditionalExpr();
        if (!elseExpr) {
            delete expr;
            delete thenExpr;
            return nullptr;
        }
        
        // TODO: 实现条件表达式
        // 暂时返回第一个表达式，后续需要添加条件表达式支持
        delete thenExpr;
        delete elseExpr;
    }
    
    return expr;
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
    Expr* expr = parseShiftExpr();
    
    while (true) {
        if (match(Lexer::eLess)) {
            Expr* right = parseShiftExpr();
            if (right) {
                expr = aNewOpBin(EOpBinType::eLt, expr, right);
            } else {
                delete expr;
                return nullptr;
            }
        } else if (match(Lexer::eLessEqual)) {
            Expr* right = parseShiftExpr();
            if (right) {
                expr = aNewOpBin(EOpBinType::eLe, expr, right);
            } else {
                delete expr;
                return nullptr;
            }
        } else if (match(Lexer::eGreater)) {
            Expr* right = parseShiftExpr();
            if (right) {
                expr = aNewOpBin(EOpBinType::eGt, expr, right);
            } else {
                delete expr;
                return nullptr;
            }
        } else if (match(Lexer::eGreaterEqual)) {
            Expr* right = parseShiftExpr();
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

/// @brief 解析语句序列（多个表达式，用分号或换行分隔）
Expr* Parser::parseStatementSequence()
{
    // 首先解析第一个表达式
    Expr* firstExpr = parseExpression();
    if (!firstExpr) {
        return nullptr;
    }
    
    // 检查是否到达文件末尾或end关键字
    int tokenType = currentTokenType();
    if (tokenType == Lexer::eEndOfFile || tokenType == Lexer::eEnd) {
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
            advance();
        } else if (tokenType == Lexer::eEnd) {
            break;
        } else {
            return nullptr;
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
    Expr* block = parseBeginEndBlock();
    if (block) {
        return block;
    }
    
    // 否则解析语句序列
    return parseStatementSequence();
}

/// @brief 解析基本表达式
Expr* Parser::parsePrimaryExpr()
{
    // 处理括号表达式
    if (match(Lexer::eLeftParen)) {
        Expr* expr = parseExpression();
        if (match(Lexer::eRightParen)) {
            return expr;
        }
        delete expr;
        return nullptr;
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