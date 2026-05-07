///
/// @file      MotionSimpleAscent.hpp
/// @brief     简单上升运动
/// @details   简单上升运动用于模拟运载火箭的上升阶段运动
/// @author    axel
/// @date      2026-04-10
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
#include "AstSim/MotionWithIntervalStep.hpp"
#include "AstCore/Time.hpp"
#include "AstCore/Object.hpp"
#include "AstSim/MotionProfile.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

/// @brief 简单上升运动模型，用于模拟运载火箭的上升阶段运动
/// @details
/// @see MotionProfile
class AST_SIM_API MotionSimpleAscent: public MotionProfile
{
public:
    AST_OBJECT(MotionSimpleAscent)
    static MotionSimpleAscent* New();
    MotionSimpleAscent();
    ~MotionSimpleAscent() override = default;
    
    /// @brief 生成特定星历
    /// @param eph 星历指针
    /// @return 错误码
    errc_t makeEphemerisSpec(ScopedPtr<Ephemeris>& eph) const override;

    /// @brief 生成简单星历
    /// @param eph 星历指针
    /// @return 错误码
    errc_t makeEphemerisSimple(ScopedPtr<Ephemeris>& eph) const override;

    /// @brief 接受访问者
    /// @param visitor 访问者
    void accept(MotionProfileVisitor& visitor) override;
    
    /// @brief 设置发射时间
    /// @param time 发射时间
    void setLaunchTime(const TimePoint& time) { launchTime_ = time; }
    
    /// @brief 获取发射时间
    /// @return 发射时间
    const TimePoint& getLaunchTime() const { return launchTime_; }
    
    /// @brief 设置是否使用场景时间
    /// @param useScenTime 是否使用场景时间
    void setUseScenTime(bool useScenTime) { useScenTime_ = useScenTime; }
    
    /// @brief 获取是否使用场景时间
    /// @return 是否使用场景时间
    bool getUseScenTime() const { return useScenTime_; }
    
    /// @brief 设置关机时间
    /// @param time 关机时间
    void setBurnoutTime(const TimePoint& time) { burnoutTime_ = time; }
    
    /// @brief 获取关机时间
    /// @return 关机时间
    const TimePoint& getBurnoutTime() const { return burnoutTime_; }
    
    /// @brief 设置发射位置
    /// @param latitude 纬度（度）
    /// @param longitude 经度（度）
    /// @param altitude 高度（米）
    void setLaunchPosition(double latitude, double longitude, double altitude)
    {
        launchLatitude_ = latitude;
        launchLongitude_ = longitude;
        launchAltitude_ = altitude;
    }
    
    /// @brief 获取发射纬度
    /// @return 发射纬度（度）
    double getLaunchLatitude() const { return launchLatitude_; }
    
    /// @brief 获取发射经度
    /// @return 发射经度（度）
    double getLaunchLongitude() const { return launchLongitude_; }
    
    /// @brief 获取发射高度
    /// @return 发射高度（米）
    double getLaunchAltitude() const { return launchAltitude_; }
    
    /// @brief 设置关机位置
    /// @param latitude 纬度（度）
    /// @param longitude 经度（度）
    /// @param altitude 高度（米）
    void setBurnoutPosition(double latitude, double longitude, double altitude)
    {
        burnoutLatitude_ = latitude;
        burnoutLongitude_ = longitude;
        burnoutAltitude_ = altitude;
    }
    
    /// @brief 获取关机纬度
    /// @return 关机纬度（度）
    double getBurnoutLatitude() const { return burnoutLatitude_; }
    
    /// @brief 获取关机经度
    /// @return 关机经度（度）
    double getBurnoutLongitude() const { return burnoutLongitude_; }
    
    /// @brief 获取关机高度
    /// @return 关机高度（米）
    double getBurnoutAltitude() const { return burnoutAltitude_; }
    
    /// @brief 设置关机速度
    /// @param velocity 关机速度（米/秒）
    void setBurnoutVelocity(double velocity) { burnoutVelocity_ = velocity; }
    
    /// @brief 获取关机速度
    /// @return 关机速度（米/秒）
    double getBurnoutVelocity() const { return burnoutVelocity_; }
    
    /// @brief 设置时间粒度
    /// @param granularity 时间粒度（秒）
    void setGranularity(double granularity) { granularity_ = granularity; }
    
    /// @brief 获取时间粒度
    /// @return 时间粒度（秒）
    double getGranularity() const { return granularity_; }
    
private:
    TimePoint launchTime_;          ///< 发射时间
    bool useScenTime_;              ///< 是否使用场景时间
    TimePoint burnoutTime_;         ///< 关机时间
    double launchLatitude_;         ///< 发射纬度（度）
    double launchLongitude_;        ///< 发射经度（度）
    double launchAltitude_;         ///< 发射高度（米）
    double burnoutLatitude_;        ///< 关机纬度（度）
    double burnoutLongitude_;       ///< 关机经度（度）
    double burnoutAltitude_;        ///< 关机高度（米）
    double burnoutVelocity_;        ///< 关机速度（米/秒）
    double granularity_;            ///< 时间粒度（秒）

};

/*! @} */

AST_NAMESPACE_END