///
/// @file      MotionBallistic.hpp
/// @brief     
/// @details   
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

/// @brief 弹道运动模型，用于模拟弹道导弹运动
class AST_SIM_API MotionBallistic: public MotionProfile
{
public:
    AST_OBJECT(MotionBallistic)
    static MotionBallistic* New();
    MotionBallistic();
    ~MotionBallistic() override = default;
    
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
    
    /// @brief 设置撞击时间
    /// @param time 撞击时间
    void setImpactTime(const TimePoint& time) { impactTime_ = time; }
    
    /// @brief 获取撞击时间
    /// @return 撞击时间
    const TimePoint& getImpactTime() const { return impactTime_; }
    
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
    
    /// @brief 设置撞击位置
    /// @param latitude 纬度（度）
    /// @param longitude 经度（度）
    /// @param altitude 高度（米）
    void setImpactPosition(double latitude, double longitude, double altitude)
    {
        impactLatitude_ = latitude;
        impactLongitude_ = longitude;
        impactAltitude_ = altitude;
    }
    
    /// @brief 获取撞击纬度
    /// @return 撞击纬度（度）
    double getImpactLatitude() const { return impactLatitude_; }
    
    /// @brief 获取撞击经度
    /// @return 撞击经度（度）
    double getImpactLongitude() const { return impactLongitude_; }
    
    /// @brief 获取撞击高度
    /// @return 撞击高度（米）
    double getImpactAltitude() const { return impactAltitude_; }
    
    /// @brief 设置发射速度
    /// @param velocity 发射速度（米/秒）
    void setLaunchVelocity(double velocity) { launchVelocity_ = velocity; }
    
    /// @brief 获取发射速度
    /// @return 发射速度（米/秒）
    double getLaunchVelocity() const { return launchVelocity_; }
    
    /// @brief 设置发射方位角
    /// @param azimuth 发射方位角（度）
    void setLaunchAzimuth(double azimuth) { launchAzimuth_ = azimuth; }
    
    /// @brief 获取发射方位角
    /// @return 发射方位角（度）
    double getLaunchAzimuth() const { return launchAzimuth_; }
    
    /// @brief 设置发射仰角
    /// @param elevation 发射仰角（度）
    void setLaunchElevation(double elevation) { launchElevation_ = elevation; }
    
    /// @brief 获取发射仰角
    /// @return 发射仰角（度）
    double getLaunchElevation() const { return launchElevation_; }
    
    /// @brief 设置发射持续时间
    /// @param duration 发射持续时间（秒）
    void setLaunchDuration(double duration) { launchDuration_ = duration; }
    
    /// @brief 获取发射持续时间
    /// @return 发射持续时间（秒）
    double getLaunchDuration() const { return launchDuration_; }
    
    /// @brief 设置发射类型
    /// @param type 发射类型
    void setLaunchType(int type) { launchType_ = type; }
    
    /// @brief 获取发射类型
    /// @return 发射类型
    int getLaunchType() const { return launchType_; }
    
    /// @brief 设置发射控制
    /// @param control 发射控制
    void setLaunchControl(int control) { launchControl_ = control; }
    
    /// @brief 获取发射控制
    /// @return 发射控制
    int getLaunchControl() const { return launchControl_; }
    
    /// @brief 设置近地点高度
    /// @param altitude 近地点高度（米）
    void setLaunchApogeeAlt(double altitude) { launchApogeeAlt_ = altitude; }
    
    /// @brief 获取近地点高度
    /// @return 近地点高度（米）
    double getLaunchApogeeAlt() const { return launchApogeeAlt_; }
    
private:
    TimePoint launchTime_;      ///< 发射时间
    TimePoint impactTime_;      ///< 撞击时间
    double launchLatitude_;     ///< 发射纬度（度）
    double launchLongitude_;    ///< 发射经度（度）
    double launchAltitude_;     ///< 发射高度（米）
    double impactLatitude_;     ///< 撞击纬度（度）
    double impactLongitude_;    ///< 撞击经度（度）
    double impactAltitude_;     ///< 撞击高度（米）
    double launchVelocity_;     ///< 发射速度（米/秒）
    double launchAzimuth_;      ///< 发射方位角（度）
    double launchElevation_;    ///< 发射仰角（度）
    double launchDuration_;     ///< 发射持续时间（秒）
    int launchType_;            ///< 发射类型
    int launchControl_;         ///< 发射控制
    double launchApogeeAlt_;    ///< 近地点高度（米）

};

/*! @} */

AST_NAMESPACE_END