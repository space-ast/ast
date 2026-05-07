///
/// @file      Axes.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-04
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
#include "AstCore/Object.hpp"
#include "AstUtil/ObjectNamed.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup Geometry
    @{
*/

class Rotation;
class KinematicRotation;
class Axes;
using PAxes = Axes*;
using HAxes = SharedPtr<Axes>;

/// @brief 计算轴系之间的旋转变换。
/// @param source 源轴系
/// @param target 目标轴系
/// @param tp 时间点
/// @param rotation 输出的旋转变换
/// @return 错误码
AST_CORE_API errc_t aAxesTransform(Axes* source, Axes* target, const TimePoint& tp, Rotation& rotation);


/// @brief 计算轴系之间的转换矩阵。
/// @param source 源轴系
/// @param target 目标轴系
/// @param tp 时间点
/// @param matrix 输出的转换矩阵
/// @return 错误码
AST_CORE_API errc_t aAxesTransform(Axes* source, Axes* target, const TimePoint& tp, Matrix3d& matrix);


/// @brief 计算轴系之间的运动学旋转变换
/// @param source 源轴系
/// @param target 目标轴系
/// @param tp 时间点
/// @param rotation 输出的运动学旋转变换
/// @return 错误码
AST_CORE_API errc_t aAxesTransform(Axes* source, Axes* target, const TimePoint& tp, KinematicRotation& rotation);


/// @brief 轴系类
/// @details 轴系类表示一个三维空间中的轴系，包含了轴系的旋转信息。
class AST_CORE_API Axes : public ObjectNamed
{
public:
    AST_OBJECT(Axes)
    ~Axes() override = default;
    
    /// @brief 获取当前轴系的父轴系
    /// @return 父轴系
    virtual Axes* getParent() const = 0;

    /// @brief 获取当前轴系相对于父轴系的旋转信息
    /// @param tp 时间点
    /// @param rotation 输出的旋转信息
    /// @return 错误码
    virtual errc_t getTransform(const TimePoint& tp, Rotation& rotation) const = 0;
    
    /// @brief 获取当前轴系相对于父轴系的运动学旋转信息
    /// @param tp 时间点
    /// @param rotation 输出的运动学旋转信息
    /// @return 错误码
    virtual errc_t getTransform(const TimePoint& tp, KinematicRotation& rotation) const = 0;
public:
    /// @brief 获取当前轴系的深度
    /// @return 深度
    int getDepth() const;

    /// @brief 获取当前轴系的祖先轴系
    /// @param depth 祖先轴系的深度
    /// @return 祖先轴系
    Axes* getAncestor(int depth) const;

public:
    /// @brief 获取当前轴系到目标轴系的旋转变换
    /// @param target 目标轴系
    /// @param tp 时间点
    /// @param rotation 输出的旋转变换
    /// @return 错误码
    A_ALWAYS_INLINE
    errc_t getTransformTo(Axes* target, const TimePoint& tp, Rotation& rotation) const
    {
        return aAxesTransform(const_cast<Axes*>(this), target, tp, rotation);
    }
    /// @brief 获取当前轴系到目标轴系的运动学旋转变换。
    /// @param target 目标轴系
    /// @param tp 时间点
    /// @param rotation 输出的运动学旋转变换
    /// @return 错误码
    A_ALWAYS_INLINE
    errc_t getTransformTo(Axes* target, const TimePoint& tp, KinematicRotation& rotation) const
    {
        return aAxesTransform(const_cast<Axes*>(this), target, tp, rotation);
    }
    /// @brief 获取源轴系到当前轴系的旋转变换。
    /// @param source 源轴系
    /// @param tp 时间点
    /// @param rotation 输出的旋转变换
    /// @return 错误码
    A_ALWAYS_INLINE
    errc_t getTransformFrom(Axes* source, const TimePoint& tp, Rotation& rotation) const
    {
        return aAxesTransform(source, const_cast<Axes*>(this), tp, rotation);
    }
    /// @brief 获取源轴系到当前轴系的运动学旋转变换。
    /// @param source 源轴系
    /// @param tp 时间点
    /// @param rotation 输出的运动学旋转变换
    /// @return 错误码
    A_ALWAYS_INLINE
    errc_t getTransformFrom(Axes* source, const TimePoint& tp, KinematicRotation& rotation) const
    {
        return aAxesTransform(source, const_cast<Axes*>(this), tp, rotation);
    }
    /// @brief 获取当前轴系相对于父轴系的旋转信息。
    /// @param tp 时间点
    /// @param rotation 输出的旋转信息
    /// @return 错误码
    A_ALWAYS_INLINE
    errc_t getTransformFromParent(const TimePoint& tp, Rotation& rotation) const
    {
        return getTransform(tp, rotation);
    }
    /// @brief 获取当前轴系相对于父轴系的运动学旋转信息。
    /// @param tp 时间点
    /// @param rotation 输出的运动学旋转信息
    /// @return 错误码
    A_ALWAYS_INLINE
    errc_t getTransformFromParent(const TimePoint& tp, KinematicRotation& rotation) const
    {
        return getTransform(tp, rotation);
    }
};

using PAxes = Axes*;                ///< 轴系指针(pointer to axes)
using HAxes = SharedPtr<Axes>;      ///< 轴系句柄(handle to axes)

/*! @} */

AST_NAMESPACE_END
