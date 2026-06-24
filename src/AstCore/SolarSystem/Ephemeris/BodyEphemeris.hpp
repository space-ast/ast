///
/// @file      BodyEphemeris.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-06
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

#pragma once

#include "AstGlobal.h"
#include "AstCore/Ephemeris.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup SolarSystem
    @{
*/

/// @brief 天体星历接口
/// @details 在星历接口的基础上增加对ICRF坐标系下的位置和速度的直接获取
/// @todo 但是对于行星卫星，他们的星历是相对于行星系的，例如月球
///       此时 `getPosVelICRF` 接口会产生数值截断误差，需要考虑如何处理
class AST_CORE_API BodyEphemeris: public Ephemeris
{
public:
    BodyEphemeris() = default;

    virtual ~BodyEphemeris() = default;

    /// @brief 获取天体在ICRF坐标系下的位置
    /// @param tp 时间点
    /// @param pos 输出位置
    /// @return errc_t 错误码
    virtual errc_t getPosICRF(const TimePoint& tp, Vector3d& pos) const = 0;

    /// @brief 获取天体在ICRF坐标系下的位置和速度
    /// @param tp 时间点
    /// @param pos 输出位置
    /// @param vel 输出速度
    /// @return errc_t 错误码
    virtual errc_t getPosVelICRF(const TimePoint& tp, Vector3d& pos, Vector3d& vel) const = 0;
public:
    virtual Frame* getFrame() const override;
    virtual errc_t getPos(const TimePoint& tp, Vector3d& pos) const override;
    virtual errc_t getPosVel(const TimePoint& tp, Vector3d& pos, Vector3d& vel) const override;
    virtual errc_t getInterval(TimeInterval& interval) const override = 0;
};


/*! @} */

AST_NAMESPACE_END
