///
/// @file      OpAssign.cpp
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

#include "OpAssign.hpp"
#include "AstUtil/Logger.hpp"
#include "AstScript/Value.hpp"
#include "AstUtil/SharedPtr.hpp"

AST_NAMESPACE_BEGIN

const char* OpAssignTypeStr[] = {
    "=",    // eAssign
    ":=",   // eDelayAssign
    "=&",   // eBindAssign
    "+=",   // eAddAssign
    "-=",   // eSubAssign
    "*=",   // eMulAssign
    "/=",   // eDivAssign
    "^=",   // ePowAssign
    "%=",   // eModAssign
    ".*=",  // eElemMulAssign
    "./=",  // eElemDivAssign
    ".^=",  // eElemPowAssign
    ".%=",  // eElemModAssign
    ".&&=",  // eElemAndAssign
    ".||=",  // eElemOrAssign
};

static_assert(sizeof(OpAssignTypeStr)/sizeof(OpAssignTypeStr[0]) == static_cast<size_t>(numOpAssignType), "OpAssignTypeStr size mismatch");

Value *OpAssign::eval() const
{
    return aDoOpAssign(op_, left_, right_);
}

std::string OpAssign::getExpression(Object *context) const
{
    return std::string(left_->getExpression(context)) + " " +
           OpAssignTypeStr[op_] + " " +
           right_->getExpression(context);
}

AST_NAMESPACE_END