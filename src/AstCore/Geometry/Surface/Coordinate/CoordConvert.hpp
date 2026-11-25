///
/// @file      CoordConvert.hpp
/// @brief
/// @details
/// @author    axel
/// @date      2026-11-25
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

/// @brief 直角坐标转纬度经度半径坐标
/// @param cart 直角坐标
/// @param llr 纬度经度半径坐标
AST_CORE_CAPI void aCartToLLR(const Vector3d& cart, LLR& llr);


/// @brief 纬度经度半径坐标转直角坐标
/// @param llr 纬度经度半径坐标
/// @param cart 直角坐标
AST_CORE_CAPI void aLLRToCart(const LLR& llr, Vector3d& cart);

/*! @} */

AST_NAMESPACE_END
