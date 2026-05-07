///
/// @file      RadiationForce.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-04-21
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
#include "ForceModel.hpp"
#include "AstCore/CelestialBody.hpp"
#include <vector>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/


enum class EShadowModel
{
    eNone,
    eCylindrical,
    eDualCone,
};


/// @brief 太阳辐射压力
/// @details 太阳辐射压力模型
class SolarRadiationPressure: public ForceModel
{
public:
    ESunPosition sunPosition_{ESunPosition::eTrue};      ///< 太阳位置
    bool detectShadowBoundaries_{false};                 ///< 是否检测阴影边界
    EShadowModel shadowModel_{EShadowModel::eNone};      ///< 阴影模型类型
    std::vector<HCelestialBody> eclipsingBodies_;        ///< 遮挡天体列表
    double atmAltForEclipse_{0.0};                       ///< 用于计算阴影边界的大气高度
};


/*! @} */

AST_NAMESPACE_END
