///
/// @file      Bounds.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-08-28
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


/// @brief 参数的上下界
template <typename T = double>
class Bounds
{
public:
    Bounds() = default;
    Bounds(T lower, T upper) : lower_(lower), upper_(upper) {}
    T lower() const { return lower_; }
    T upper() const { return upper_; }
    T& lower() { return lower_; }
    T& upper() { return upper_; }
    void setLower(T lower) { lower_ = lower; }
    void setUpper(T upper) { upper_ = upper; }
private:
    T lower_{};
    T upper_{};
};


using DoubleBounds = Bounds<double>;
using IntBounds = Bounds<int>;


/*! @} */

AST_NAMESPACE_END
