///
/// @file      ScStateCalcAngle.cpp
/// @brief     两向量夹角计算实现
/// @details   将两个向量统一到同一参考轴系后，用 aAngle 计算夹角。
/// @author    axel
/// @date      2026-11-02
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

#include "ScStateCalcAngle.hpp"
#include "AstCore/TimePoint.hpp"
#include "AstUtil/Logger.hpp"

AST_NAMESPACE_BEGIN

errc_t ScStateCalcAngle::calculate(const SpacecraftState& scState, double& result)
{
    Vector* vector1 = this->getVector1();
    Vector* vector2 = this->getVector2();
    if(!vector1 || !vector2)
    {
        aError(_("向量未配置"));
        return eErrorNullPtr;
    }

    TimePoint epoch;
    errc_t rc = scState.getStateEpoch(epoch);
    if(rc != eNoError)
        return rc;

    // 两个向量统一到第一个向量的参考轴系下求夹角
    Axes* axes = vector1->getAxes();
    if(!axes)
    {
        aError(_("向量 '%s' 缺少参考轴系"), vector1->getName().c_str());
        return eErrorNullPtr;
    }

    Vector3d vec1;
    rc = vector1->getVectorIn(axes, epoch, vec1);
    if(rc != eNoError)
        return rc;

    Vector3d vec2;
    rc = vector2->getVectorIn(axes, epoch, vec2);
    if(rc != eNoError)
        return rc;

    result = vec1.angle(vec2);
    return eNoError;
}

AST_NAMESPACE_END
