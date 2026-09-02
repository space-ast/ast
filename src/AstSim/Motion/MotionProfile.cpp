///
/// @file      MotionProfile.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-13
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

#include "MotionProfile.hpp"
#include "MotionTwoBody.hpp"
#include "MotionHPOP.hpp"
#include "MotionSPICE.hpp"
#include "MotionBallistic.hpp"
#include "MotionSimpleAscent.hpp"

AST_NAMESPACE_BEGIN


MotionHPOP *MotionProfile::toHPOP() const
{
    return aobject_cast<MotionHPOP*>(const_cast<MotionProfile*>(this));
}

MotionTwoBody *MotionProfile::toTwoBody() const
{
    return aobject_cast<MotionTwoBody*>(const_cast<MotionProfile*>(this));
}

MotionSPICE *MotionProfile::toSPICE() const
{
    return aobject_cast<MotionSPICE*>(const_cast<MotionProfile*>(this));
}

MotionBallistic *MotionProfile::toBallistic() const
{
    return aobject_cast<MotionBallistic*>(const_cast<MotionProfile*>(this));
}

MotionSimpleAscent *MotionProfile::toSimpleAscent() const
{
    return aobject_cast<MotionSimpleAscent*>(const_cast<MotionProfile*>(this));
}

Ephemeris *MotionProfile::createEphemeris() const
{
    /*
    先尝试创建特殊星历类型，失败则创建简单的时序星历
    */
    ScopedPtr<Ephemeris> eph;
    errc_t rc = createEphemeris(eph);
    A_UNUSED(rc);
    return eph.release();
}

errc_t MotionProfile::createEphemeris(SharedPtr<Ephemeris> &eph) const
{
    ScopedPtr<Ephemeris> ephTemp;
    errc_t rc = createEphemeris(ephTemp);
    if (rc == eNoError)
        eph = ephTemp.release();
    return rc;
}

errc_t MotionProfile::createEphemeris(ScopedPtr<Ephemeris> &eph) const
{
    errc_t rc = makeEphemerisSpec(eph);
    if (rc != eNoError)
    {
        return makeEphemerisSimple(eph);
    }
    return rc;
}

AST_NAMESPACE_END