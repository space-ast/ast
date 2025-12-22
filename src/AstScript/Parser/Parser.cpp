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
#include "AstScript/OpBin.hpp"
#include "AstScript/OpAssign.hpp"
#include "AstScript/OpUnary.hpp"
#include "AstScript/Variable.hpp"
#include "AstScript/Symbol.hpp"
#include "Scanner.hpp"
#include "Lexer.hpp"

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
size_t Parser::getLine() const { return lexer_.getLine(); }
    
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

/// @brief 解析赋值表达式
Expr* Parser::parseAssignExpr()
{
    Expr* expr = parseConditionalExpr();
    
    if (match(Lexer::eEqual)) {
        Expr* right = parseAssignExpr();
        if (right) {
            return aNewOpAssign(OpAssignType::eAssign, expr, right);
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
            expr = aNewOpBin(OpBinType::eOr, expr, right);
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
            expr = aNewOpBin(OpBinType::eAnd, expr, right);
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
    while (true) {
        if (match(Lexer::ePipe)) {
            Expr* right = parseBitwiseXorExpr();
            if (right) {
                expr = aNewOpBin(OpBinType::eBitOr, expr, right);
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

/// @brief 解析位异或表达式
Expr* Parser::parseBitwiseXorExpr()
{
    Expr* expr = parseBitwiseAndExpr();
    while (true) {
        if (match(Lexer::eXor)) {
            Expr* right = parseBitwiseAndExpr();
            if (right) {
                expr = aNewOpBin(OpBinType::eBitXor, expr, right);
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
                expr = aNewOpBin(OpBinType::eBitAnd, expr, right);
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
                expr = aNewOpBin(OpBinType::eEq, expr, right);
            } else {
                delete expr;
                return nullptr;
            }
        } else if (match(Lexer::eBangEqual)) {
            Expr* right = parseRelationalExpr();
            if (right) {
                expr = aNewOpBin(OpBinType::eNe, expr, right);
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
                expr = aNewOpBin(OpBinType::eLt, expr, right);
            } else {
                delete expr;
                return nullptr;
            }
        } else if (match(Lexer::eLessEqual)) {
            Expr* right = parseShiftExpr();
            if (right) {
                expr = aNewOpBin(OpBinType::eLe, expr, right);
            } else {
                delete expr;
                return nullptr;
            }
        } else if (match(Lexer::eGreater)) {
            Expr* right = parseShiftExpr();
            if (right) {
                expr = aNewOpBin(OpBinType::eGt, expr, right);
            } else {
                delete expr;
                return nullptr;
            }
        } else if (match(Lexer::eGreaterEqual)) {
            Expr* right = parseShiftExpr();
            if (right) {
                expr = aNewOpBin(OpBinType::eGe, expr, right);
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
                expr = aNewOpBin(OpBinType::eBitLeftShift, expr, right);
            } else {
                delete expr;
                return nullptr;
            }
        } else if (match(Lexer::eGreaterGreater)) {
            Expr* right = parseAdditiveExpr();
            if (right) {
                expr = aNewOpBin(OpBinType::eBitRightShift, expr, right);
            } else {
                delete expr;
                return nullptr;
            }
        } else if (match(Lexer::eGreaterGreaterGreater)) {
            Expr* right = parseAdditiveExpr();
            if (right) {
                expr = aNewOpBin(OpBinType::eBitURightShift, expr, right);
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
                expr = aNewOpBin(OpBinType::eAdd, expr, right);
            } else {
                delete expr;
                return nullptr;
            }
        } else if (match(Lexer::eMinus)) {
            Expr* right = parseMultiplicativeExpr();
            if (right) {
                expr = aNewOpBin(OpBinType::eSub, expr, right);
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
                expr = aNewOpBin(OpBinType::eMul, expr, right);
            } else {
                delete expr;
                return nullptr;
            }
        } else if (match(Lexer::eSlash)) {
            Expr* right = parseExponentiationExpr();
            if (right) {
                expr = aNewOpBin(OpBinType::eDiv, expr, right);
            } else {
                delete expr;
                return nullptr;
            }
        } else if (match(Lexer::ePercent)) {
            Expr* right = parseExponentiationExpr();
            if (right) {
                expr = aNewOpBin(OpBinType::eMod, expr, right);
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
            return aNewOpBin(OpBinType::ePow, expr, right);
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
            return aNewOpUnary(OpUnaryType::eNeg, expr);
        }
        return nullptr;
    } else if (match(Lexer::eBang)) {
        Expr* expr = parseUnaryExpr();
        if (expr) {
            return aNewOpUnary(OpUnaryType::eNot, expr);
        }
        return nullptr;
    } else if (match(Lexer::eTilde)) {
        Expr* expr = parseUnaryExpr();
        if (expr) {
            return aNewOpUnary(OpUnaryType::eBitNot, expr);
        }
        return nullptr;
    }
    
    return parsePrimaryExpr();
}

/// @brief 解析基本表达式
Expr* Parser::parsePrimaryExpr()
{
    if (match(Lexer::eLeftParen)) {
        Expr* expr = parseExpression();
        if (match(Lexer::eRightParen)) {
            return expr;
        }
        delete expr;
        return nullptr;
    }
    
    if (currentTokenType() == Lexer::eNumber) {
        std::string numStr = currentLexeme().to_string();
        
        // @todo: 下面的逻辑有待优化

        // 检查是否为浮点数
        if (numStr.find_first_of("eE.") != std::string::npos && numStr[1] != 'x') 
        {
            double value = std::strtod(numStr.data(), nullptr);
            advance();
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
            
            advance();
            // 检查是否超出int范围
            if (value < std::numeric_limits<int>::min() || value > std::numeric_limits<int>::max()) {
                // 超出int范围
                return nullptr;
            }
            return new ValInt(static_cast<int>(value));
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
    
    Expr* expr = context.parseExpression();
    
    // 检查是否解析到文件末尾
    if (expr && context.currentTokenType() != Lexer::eEndOfFile) {
        delete expr;
        return nullptr;
    }
    
    return expr;
}

AST_NAMESPACE_END