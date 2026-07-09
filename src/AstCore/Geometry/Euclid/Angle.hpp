///
/// @file      Angle.hpp
/// @brief
/// @details
/// @author    axel
/// @date      2026-03-09
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
#include "AstUtil/Object.hpp"
#include "AstUtil/ObjectNamed.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup Geometry
    @{
*/

/// @brief 角度抽象基类
/// @details 角度是一种几何元素，表示一个随时间变化的角度量。
///          所有具体角度类型（如常量角度、计算角度等）均派生自此类。
///          角度值以弧度为单位。
class AST_CORE_API Angle : public ObjectNamed
{
public:
    AST_OBJECT(Angle)

    Angle() = default;
    ~Angle() override = default;

    /// @brief 获取指定时间点的角度值
    /// @param tp 时间点
    /// @param value 输出参数，角度值（弧度）
    /// @return 错误码
    virtual errc_t getAngle(const TimePoint& tp, double& value) const = 0;

    /// @brief 获取指定时间点的角度值和角速度
    /// @param tp 时间点
    /// @param value 输出参数，角度值（弧度）
    /// @param angVel 输出参数，角速度（弧度/秒）
    /// @return 错误码
    virtual errc_t getAngle(const TimePoint& tp, double& value, double& angVel) const = 0;

    /// @brief 获取指定时间点的角度值
    /// @param tp 时间点
    /// @return 角度值（弧度），出错时返回 0.0
    double getAngle(const TimePoint& tp) const;
};

using PAngle = Angle*;                ///< 角度指针(pointer to angle)
using HAngle = SharedPtr<Angle>;      ///< 角度句柄(handle to angle)

/*! @} */

AST_NAMESPACE_END
