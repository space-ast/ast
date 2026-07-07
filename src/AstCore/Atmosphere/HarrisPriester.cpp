///
/// @file      HarrisPriester.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-06-27
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

#include "HarrisPriester.hpp"
#include "AstWeather/HarrisPriester.hpp"
#include "AstMath/Vector.hpp"
#include "AstUtil/Logger.hpp"
#include "AstCore/BodyPosition.hpp"
#include "AstCore/CelestialBody.hpp"
#include <cassert>

AST_NAMESPACE_BEGIN


HarrisPriester::HarrisPriester(Frame *frame, BodyShape *bodyShape, CelestialBody* sun, double f107)
    : AtmosphereBase(frame, bodyShape)
    , sun_(sun)
    , f107_(f107)
{
}

double HarrisPriester::getDensity(const TimePoint &tp, const Vector3d &posInBodyFixed) const
{
    assert(sun_ != nullptr);

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
    double alt = getAltitude(posInBodyFixed);
    return aHarrisPriester(posInBodyFixed, sun, alt, f107_, n_);
}

AST_NAMESPACE_END
