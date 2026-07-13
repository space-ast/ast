///
/// @file      BlockSRP.cpp
/// @brief     太阳辐射压力函数块实现
/// @details   ~
/// @author    axel
/// @date      2026-01-19
/// @copyright 版权所有 (C) 2026-present, ast项目.
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

#include "BlockSRP.hpp"
#include "AstCore/BlockAstro.hpp"
#include "AstCore/CelestialBody.hpp"
#include "AstCore/BodyPosition.hpp"
#include "AstCore/Frame.hpp"
#include "AstCore/BuiltinFrame.hpp"
#include "AstUtil/Identifier.hpp"
#include "AstUtil/Constants.h"
#include "AstMath/Vector.hpp"
#include "AstMath/MathOperator.hpp"
#include "AstCore/NoneEclipseCalculator.hpp"

AST_NAMESPACE_BEGIN

using namespace math;



BlockSRP::BlockSRP()
    : BlockSRP(nullptr, 0, 0, nullptr)
{
}

BlockSRP::BlockSRP(EclipseCalculator* eclipseCalculator, double cr, double srpArea, Frame* propagationFrame)
    : BlockDerivative{}
    , position_{&vectorBuffer_}
    , velocity_{&vectorBuffer_}
    , accSRP_{&vectorBuffer_}
    , velocityDerivative_{&vectorBuffer_}
    , vectorBuffer_{}
    , eclipseCalculator_{eclipseCalculator}
    , cr_{cr}
    , srpArea_{srpArea}
    , propagationFrame_{propagationFrame}
{
    static auto identifierPos = aIdentifier(kIdentifierPos);
    static auto identifierAccSRP = aIdentifier(kIdentifierAccSRP);
    static auto identifierVel = aIdentifier(kIdentifierVel);
    static auto identifierMass = aIdentifier(kIdentifierMass);
    
    if(!eclipseCalculator_)
        eclipseCalculator_ = new NoneEclipseCalculator(aGetSun());

    inputPorts_ = {
        // 位置（预报坐标系下）
        {
            identifierPos,
            (signal_t*)&position_,
            3,
            DataPort::eDouble
        },
        // 速度（预报坐标系下）
        {
            identifierVel,
            (signal_t*)&velocity_,
            3,
            DataPort::eDouble
        },
        // 质量
        {
            identifierMass,
            (signal_t*)&mass_,
            1,
            DataPort::eDouble
        }
    };

    outputPorts_ = {
        // 太阳辐射压力加速度
        {
            identifierAccSRP,
            (signal_t*)&accSRP_,
            3,
            DataPort::eDouble
        }
    };

    derivativePorts_ = {
        // 速度导数
        {
            identifierVel,
            (signal_t*)&velocityDerivative_,
            3,
            DataPort::eDouble
        }
    };
}

BlockSRP::~BlockSRP()
{
    if(eclipseCalculator_)
    {
        delete eclipseCalculator_;
        eclipseCalculator_ = nullptr;
    }    
}

errc_t BlockSRP::run(const SimTime& simTime)
{
    assert(eclipseCalculator_);
    assert(propagationFrame_);

    auto& tp = simTime.timePoint();
    auto sun = eclipseCalculator_->lightSource();
    assert(sun);
    /// @todo 目前总是使用真太阳位置进行计算，是否需要考虑使用视太阳位置进行计算？
    double lightingRatio = eclipseCalculator_->getLightingRatio(tp, *position_, propagationFrame_);
    if(lightingRatio == 0)
    {
        *accSRP_ = Vector3d::Zero();
        return eNoError;
    }
    
    Vector3d scToSun;  // 航天器指向太阳的向量

    const EAberrationFlags aberrationFlags = EAberrationFlags::eCN_S;
    errc_t err;
    if(sunPosition_ == ESunPosition::eApparent)
    {
        err = aApparentPositionInFrame(sun, tp, propagationFrame_, *position_, *velocity_, aberrationFlags, scToSun, nullptr);
    }
    else if(sunPosition_ == ESunPosition::eApparentSunToTrueCB)
    {
        Vector3d sunPos;
        err = aApparentPositionInFrame(sun, tp, propagationFrame_, Vector3d::Zero(), Vector3d::Zero(), aberrationFlags, sunPos, nullptr);
        scToSun = sunPos - *position_;
    }
    else // if(sunPosition_ == ESunPosition::eTrue)
    {
        Vector3d sunPos;
        err = sun->getPosIn(propagationFrame_, tp, sunPos);
        scToSun = sunPos - *position_;
    }
    
    if (A_UNLIKELY(err != eNoError))
    {
        aError("failed to calculate sun position");
        return err;
    }

    double rSqr = scToSun.squaredNorm();
    double r = std::sqrt(rSqr);

    double mass = *mass_;
    if (A_UNLIKELY(mass <= 0))
    {
        aError("spacecraft mass is zero or negative (%f), cannot compute SRP acceleration", mass);
        *accSRP_ = Vector3d::Zero();
        return eErrorInvalidParam;
    }

    // 太阳辐射压力加速度
    // a_srp = Cr * (A/m) * P_1AU * (AU/r)^2 * (sunToSc / r)
    //       = Cr * A/m * P_1AU * AU^2 / r^3 * sunToSc
    // 方向：远离太阳（从太阳指向航天器）
    double factor = - lightingRatio * cr_ * srpArea_  / mass * kSolarPressureAt1AU * (kAU * kAU) / (r * rSqr);
    *accSRP_ = factor * scToSun;

    // 添加到速度导数上
    *velocityDerivative_ += *accSRP_;

    return eNoError;
}

AST_NAMESPACE_END
