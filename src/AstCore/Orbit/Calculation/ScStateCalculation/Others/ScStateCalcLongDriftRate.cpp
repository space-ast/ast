///
/// @file      ScStateCalcLongDriftRate.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-05-02
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

#include "ScStateCalcLongDriftRate.hpp"
#include "AstCore/OrbitElement.hpp"
#include "AstCore/OrbitalPrecession.hpp"
#include "AstCore/CelestialBody.hpp"
#include "AstCore/TimePoint.hpp"
#include "AstMath/KinematicRotation.hpp"
#include "AstUtil/Constants.hpp"
#include "AstUtil/Logger.hpp"

AST_NAMESPACE_BEGIN

errc_t ScStateCalcLongDriftRate::calculate(const SpacecraftState& scState, double& result)
{
    // 计算时的中心天体仅由计算量自身的配置决定，未配置时使用默认天体（地球），不从输入状态所在坐标系推断
    CelestialBody* body = this->body();
    if(!body)
    {
        body = aGetEarth();
        if(!body)
        {
            aError(_("未配置中心天体"));
            return eErrorNullPtr;
        }
    }

    // 将状态转换到中心天体的惯性系后再计算轨道根数
    CartState cartState;
    errc_t rc = scState.getStateInBodyInertial(body, cartState);
    if(rc != eNoError)
        return rc;

    const double gm = body->getGM();
    const double j2 = body->getJ2();
    double rb = body->getGravityRefDistance();
    if(rb <= 0)
        rb = body->getRadius();

    ModOrbElem orbElem{};
    rc = aCartToModOrbElem(cartState.pos(), cartState.vel(), gm, orbElem);
    if(rc != eNoError)
        return rc;

    const double sma = orbElem.getSMA();
    const double ecc = orbElem.getEcc();
    const double inc = orbElem.getInc();
    if(!(sma > 0) || ecc < 0 || ecc >= 1)
    {
        aError(_("轨道根数无效：半长轴 %.6g m，偏心率 %.6g"), sma, ecc);
        return eErrorInvalidParam;
    }

    // 考虑 J2长期项后的平经度变化率（平近点角、近点幅角、升交点赤经变化率之和）
    const double orbitRate = aMeanArgLatRate(gm, j2, rb, sma, ecc, inc)
                           + aRAANRate(gm, j2, rb, sma, ecc, inc);

    // 中心天体自转角速率
    double bodyRate = 0.0;
    if(body->isEarth())
    {
        // @todo 这里需要根据地球的指向模型来计算当前时刻的自转角速率
        bodyRate = kEarthAngVel;
    }
    else
    {
        BodyOrientation* orientation = body->getOrientation();
        if(!orientation)
        {
            aError(_("天体 '%s' 缺少姿态模型，无法确定自转角速率"), body->getName().c_str());
            return eErrorNullPtr;
        }
        TimePoint epoch;
        rc = scState.getStateEpoch(epoch);
        if(rc != eNoError)
            return rc;
        KinematicRotation rotation;
        orientation->getICRFToFixedTransform(epoch, rotation);
        bodyRate = rotation.getRotationRate().norm();
    }

    result = orbitRate - bodyRate;
    return eNoError;
}

AST_NAMESPACE_END
