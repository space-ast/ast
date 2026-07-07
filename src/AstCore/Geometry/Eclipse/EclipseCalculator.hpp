///
/// @file      EclipseCalculator.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-06-10
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
#include "AstCore/CelestialBody.hpp"
#include <vector>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/


/// @brief 阴影光照计算器
class AST_CORE_API EclipseCalculator
{
public:
    EclipseCalculator() = default;
    explicit EclipseCalculator(CelestialBody* lightSource)
        : lightSource_(lightSource)
    {
    }
    virtual ~EclipseCalculator() = default;

    /// @brief 计算光照强度
    /// @param time 时间点
    /// @param position 观测者位置（相对于观测者所在参考系）
    /// @param frame 观测者所在参考系
    /// @return 光照强度, 0: 全阴影, 1: 全光照，0-1: 部分遮挡/光照
    virtual double getLightingRatio(const TimePoint& time, const Vector3d& position, Frame* frame) = 0;

    /// @brief 获取光源体
    CelestialBody* lightSource() const {return lightSource_;}

    /// @brief 设置光源体
    void setLightSource(CelestialBody* lightSource) {lightSource_ = lightSource;}

    /// @brief 设置遮挡体列表
    void setOccultingBodies(const std::vector<HCelestialBody>& occultingBodies) {occultingBodies_ = occultingBodies;}

    /// @brief 添加遮挡体
    void addOccultingBody(HCelestialBody occultingBody) {occultingBodies_.push_back(occultingBody);}
protected:
    HCelestialBody lightSource_{nullptr};           ///< 光源体
    std::vector<HCelestialBody> occultingBodies_;   ///< 阴影体列表
};



/*! @} */

AST_NAMESPACE_END
