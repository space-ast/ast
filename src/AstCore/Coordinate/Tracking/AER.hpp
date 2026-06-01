///
/// @file      AER.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-06-01
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
#include "AstMath/Vector.hpp"
#include "TrackingCoordinates.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/


/// @brief 通过NED坐标计算AER
AST_CORE_CAPI void aNEDToAER(const Vector3d& ned, AER& aer);

/// @brief 通过AER坐标计算NED坐标
AST_CORE_CAPI void aAERToNED(const AER& aer, Vector3d& ned);

/// @brief 通过ENU坐标计算AER
AST_CORE_CAPI void aENUToAER(const Vector3d& enu, AER& aer);

/// @brief 通过AER坐标计算ENU坐标
AST_CORE_CAPI void aAERToENU(const AER& aer, Vector3d& enu);


// --------------------------------
// MATLAB Mapping Toolbox 同名函数
// ---------------------------------


A_ALWAYS_INLINE AER ned2aer(const Vector3d& ned)
{
    AER aer;
    aNEDToAER(ned, aer);
    return aer;
}

A_ALWAYS_INLINE Vector3d aer2ned(const AER& aer)
{
    Vector3d ned;
    aAERToNED(aer, ned);
    return ned;
}


A_ALWAYS_INLINE AER enu2aer(const Vector3d& enu)
{
    AER aer;
    aENUToAER(enu, aer);
    return aer;
}


A_ALWAYS_INLINE Vector3d aer2enu(const AER& aer)
{
    Vector3d enu;
    aAERToENU(aer, enu);
    return enu;
}

/*! @} */

AST_NAMESPACE_END


