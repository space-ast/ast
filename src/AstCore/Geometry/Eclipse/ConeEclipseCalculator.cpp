///
/// @file      ConeEclipseCalculator.cpp
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

#include "ConeEclipseCalculator.hpp"
#include "Eclipse.hpp"
#include "AstCore/CelestialBody.hpp"
#include "AstCore/Point.hpp"

AST_NAMESPACE_BEGIN

double ConeEclipseCalculator::getLightingRatio(const TimePoint& time, const Vector3d& position, Frame* frame)
{
    return getLightingRatio(time, position, frame, nullptr);
}

double ConeEclipseCalculator::getLightingRatio(const TimePoint& time, const Vector3d& position, Frame* frame,
                                               CelestialBody** obstructionOut)
{
    // 无光源或无遮挡体，视为全光照
    if (!lightSource_ || occultingBodies_.empty())
    {
        return 1.0;
    }

    // 获取光源在观测者坐标系下的位置
    Vector3d lightSourcePos;
    if (lightSource_->getPosIn(frame, time, lightSourcePos) != eNoError)
    {
        return 1.0; // 获取失败，保守返回全光照
    }

    double minRatio = 1.0; // 初始为全光照，取所有遮挡体中最暗的结果
    double lightingSourceRadius = lightSource_->getRadius();

    for (auto& occultingBody : occultingBodies_)
    {
        if (!occultingBody || occultingBody == lightSource_)
        {
            continue;
        }

        // 获取遮挡体在观测者坐标系下的位置
        Vector3d occultingBodyPos;
        if (occultingBody->getPosIn(frame, time, occultingBodyPos) != eNoError)
        {
            continue;
        }

        // 观测者相对于遮挡体中心的位置
        Vector3d obsRelativeToOcc = position - occultingBodyPos;

        // 光源相对于遮挡体中心的位置
        Vector3d lightRelativeToOcc = lightSourcePos - occultingBodyPos;

        /// @todo 计算遮挡时总是假设遮挡体是个完美球形，如果是其他形状，需要改进计算方法
        double ratio = aLightingRatio_DualConeModel(obsRelativeToOcc, lightRelativeToOcc,
                                                     lightingSourceRadius, occultingBody->getRadius());
        if (ratio < minRatio)
        {
            minRatio = ratio;
            if (obstructionOut)
            {
                *obstructionOut = occultingBody.get();
            }
        }
    }

    return minRatio;
}

AST_NAMESPACE_END
