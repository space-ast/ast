///
/// @file      Bracket.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-05-04
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

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

/// @brief 括号区间，包含左右边界和左右边界对应的值
class Bracket
{
public:
    Bracket(double leftAbscissa, double leftValue, double rightAbscissa, double rightValue)
        : leftAbscissa_(leftAbscissa), leftValue_(leftValue), rightAbscissa_(rightAbscissa), rightValue_(rightValue)
    {}
    /// @brief 获取左边界对应的横坐标
    double leftAbscissa() const{return leftAbscissa_;}
    /// @brief 获取左边界对应的值(函数值)
    double leftValue() const{return leftValue_;}
    /// @brief 获取右边界对应的横坐标
    double rightAbscissa() const{return rightAbscissa_;}
    /// @brief 获取右边界对应的值(函数值)
    double rightValue() const{return rightValue_;}

protected:
    double leftAbscissa_;
    double leftValue_;
    double rightAbscissa_;
    double rightValue_;
};

/*! @} */

AST_NAMESPACE_END

