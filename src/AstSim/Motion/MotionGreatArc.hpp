///
/// @file      MotionGreatArc.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-04-13
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
#include "MotionProfile.hpp"
#include "AstCore/GeodeticPoint.hpp"
#include <vector>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/


class WayPoint
{
public:
    double time() const {return time_;}
public:
    double time_;             ///< 时间
    GeodeticPoint position_;  ///< 位置
    double speed_;            ///< 速度
    double acceleration_;     ///< 加速度
    double turnRadius_;       ///< 转弯半径
};


class MotionGreatArc;
using PMotionGreatArc = MotionGreatArc*;
using HMotionGreatArc = SharedPtr<MotionGreatArc>;

class AST_SIM_API MotionGreatArc : public MotionProfile
{
public:
    static PMotionGreatArc New();
    MotionGreatArc();
    ~MotionGreatArc();
public:
    errc_t makeEphemerisSpec(ScopedPtr<Ephemeris>& eph) const override;
    errc_t makeEphemerisSimple(ScopedPtr<Ephemeris>& eph) const override;
    void accept(MotionProfileVisitor& visitor) override;
public:
    using WayPointVector = std::vector<WayPoint>;
    void setStartTime(const TimePoint& tp);
    errc_t getStartTime(TimePoint& tp) const;
    errc_t getStopTime(TimePoint& tp) const;
    errc_t getInterval(TimeInterval& interval) const;
    const WayPointVector& getWayPoints() const{return wayPoints_;}
    void setWayPoints(const WayPointVector& wayPoints){wayPoints_ = wayPoints;}
private:
    SharedPtr<EventTime> startTime_{};       ///< 开始时间
    WayPointVector wayPoints_{};             ///< 路径点
};

/*! @} */

AST_NAMESPACE_END
