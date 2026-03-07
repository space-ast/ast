///
/// @file      Frame.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-02-22
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

AST_NAMESPACE_BEGIN

/*!
    @addtogroup Geometry
    @{
*/

class Transform;
class KinematicTransform;
class Frame;
class Axes;
class Point;

/// @brief 计算坐标系之间的变换。
/// @param source 源坐标系
/// @param target 目标坐标系
/// @param tp 时间点
/// @param transform 输出的变换
/// @return 错误码
AST_CORE_API err_t aFrameTransform(Frame* source, Frame* target, const TimePoint& tp, Transform& transform);

/// @brief 计算坐标系之间的运动学变换。
/// @param source 源坐标系
/// @param target 目标坐标系
/// @param tp 时间点
/// @param transform 输出的运动学变换
/// @return 错误码
AST_CORE_API err_t aFrameTransform(Frame* source, Frame* target, const TimePoint& tp, KinematicTransform& transform);


/// @brief 坐标系类
/// @details 坐标系类表示一个三维空间中的坐标系，包含了坐标系的变换信息。
class Frame: public Object
{
public:
    Frame() = default;
    ~Frame() override= default;
    /// @brief 获取当前坐标系的父坐标系
    /// @return 父坐标系
    virtual Frame* getParent() const = 0;
    /// @brief 获取当前坐标系相对于父坐标系的变换
    /// @param tp 时间点
    /// @param transform 输出的变换
    /// @return 错误码
    virtual err_t getTransform(Transform& transform) const = 0;
    /// @brief 获取当前坐标系相对于父坐标系的运动学变换
    /// @param tp 时间点
    /// @param transform 输出的运动学变换
    /// @return 错误码
    virtual err_t getTransform(const TimePoint& tp, KinematicTransform& transform) const = 0;
    /// @brief 获取当前坐标系的轴系。
    /// @return 轴系
    virtual Axes* getAxes() const = 0;
    /// @brief 获取当前坐标系的原点。
    /// @return 原点
    virtual Point* getOrigin() const = 0;
public:
    /// @brief 获取当前坐标系到目标坐标系的变换。
    /// @param target 目标坐标系
    /// @param tp 时间点
    /// @param transform 输出的变换
    /// @return 错误码
    err_t getTransformTo(Frame* target, const TimePoint& tp, Transform& transform) const
    {
        return aFrameTransform(const_cast<Frame*>(this), target, tp, transform);
    }
    /// @brief 获取当前坐标系到目标坐标系的运动学变换。
    /// @param target 目标坐标系
    /// @param tp 时间点
    /// @param transform 输出的运动学变换
    /// @return 错误码
    err_t getTransformTo(Frame* target, const TimePoint& tp, KinematicTransform& transform) const
    {
        return aFrameTransform(const_cast<Frame*>(this), target, tp, transform);
    }
    /// @brief 获取源坐标系到当前坐标系的变换。
    /// @param source 源坐标系
    /// @param tp 时间点
    /// @param transform 输出的变换
    /// @return 错误码
    err_t getTransformFrom(Frame* source, const TimePoint& tp, Transform& transform) const
    {
        return aFrameTransform(source, const_cast<Frame*>(this), tp, transform);
    }
    /// @brief 获取源坐标系到当前坐标系的运动学变换。
    /// @param source 源坐标系
    /// @param tp 时间点
    /// @param transform 输出的运动学变换
    /// @return 错误码
    err_t getTransformFrom(Frame* source, const TimePoint& tp, KinematicTransform& transform) const
    {
        return aFrameTransform(source, const_cast<Frame*>(this), tp, transform);
    }
};

using HFrame = SharedPtr<Frame>;
using PFrame = Frame*;

/*! @} */

AST_NAMESPACE_END
