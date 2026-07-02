///
/// @file      JacchiaRoberts.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-06-26
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

#include "JacchiaRoberts.hpp"
#include "AstWeather/Atmosphere.hpp"
#include "AstCore/TimePoint.hpp"
#include "AstCore/JulianDate.hpp"
#include "AstCore/BodyPosition.hpp"
#include "AstCore/CelestialBody.hpp"
#include "AstCore/Frame.hpp"
#include "AstMath/Vector.hpp"

AST_NAMESPACE_BEGIN

JacchiaRoberts::JacchiaRoberts(Frame *frame, BodyShape *bodyShape, CelestialBody *sun, double f107Daily, double f107Average, double kp)
    : AtmosphereBase(frame, bodyShape)
    , sun_{sun}
    , F107Daily_{f107Daily}
    , F107Average_{f107Average}
    , kp_{kp}
{
}

/// @brief 计算外大气层温度
/// @note 参考GMAT的函数JacchiaRobertsAtmosphere::JacchiaRoberts
static double aExospehricTemperature(double f107, double f107a)
{
    return 379.0 + 3.24 * f107a + 1.3 * (f107 - f107a);
}

double JacchiaRoberts::getDensity(const TimePoint &tp, const Vector3d &posInBodyFixed) const
{
    assert(sun_ != nullptr);
    assert(bodyShape_ != nullptr);
    assert(frame_ != nullptr);

    const EAberrationFlags aberrationFlags = EAberrationFlags::eCN_S;
    Vector3d sun;
    errc_t err;
    if(sunPosition_ == ESunPosition::eApparent || sunPosition_ == ESunPosition::eApparentSunToTrueCB)
    {
        // @todo 处理 eApparentSunToTrueCB 的情况
        err = aApparentPositionInFrame(sun_, tp, frame_, Vector3d::Zero(), Vector3d::Zero(), aberrationFlags, sun, nullptr);
    }
    else
    {
        err = sun_->getPosIn(frame_, tp, sun);
    }
    if (A_UNLIKELY(err != eNoError))
    {
        aError("failed to calculate sun position");
        return 0;
    }

    double alt = this->getAltitude(posInBodyFixed);
    double xtemp = aExospehricTemperature(F107Daily_, F107Average_);
    JulianDate jdTAI = tp.toTAI();
    double a1_time = jdTAI.daysFromJulianDate(2430000);
    // @fixme 
    // 根据 val_atm_jr 的注释， JacchiaRoberts 函数需要传入相对于TOD GCI系下的坐标
    // 虽然根据 JacchiaRoberts 函数里面的逻辑实现分析可知：输入地固系坐标和输入TOD GCI坐标的计算结果应该差别很小
    // 但是这样调用和文档中的描述不一致
    double rho = aJacchiaRoberts(alt, posInBodyFixed, sun, a1_time, kp_, xtemp);
    return rho;
}


AST_NAMESPACE_END

