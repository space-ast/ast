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
AST_CORE_API errc_t aAxesTransform(Axes& source, Axes& target, const TimePoint& tp, Rotation& rotation);
AST_CORE_API errc_t aAxesTransform(Axes* source, Axes* target, const TimePoint& tp, Rotation& rotation);


/// @brief 计算轴系之间的转换矩阵。
/// @param source 源轴系
/// @param target 目标轴系
/// @param tp 时间点
/// @param matrix 输出的转换矩阵
/// @return 错误码
AST_CORE_API errc_t aAxesTransform(Axes& source, Axes& target, const TimePoint& tp, Matrix3d& matrix);
AST_CORE_API errc_t aAxesTransform(Axes* source, Axes* target, const TimePoint& tp, Matrix3d& matrix);


/// @brief 计算轴系之间的运动学旋转变换
/// @param source 源轴系
/// @param target 目标轴系
/// @param tp 时间点
/// @param rotation 输出的运动学旋转变换
/// @return 错误码
AST_CORE_API errc_t aAxesTransform(Axes& source, Axes& target, const TimePoint& tp, KinematicRotation& rotation);
AST_CORE_API errc_t aAxesTransform(Axes* source, Axes* target, const TimePoint& tp, KinematicRotation& rotation);


/// @brief 轴系抽象基类
/// @details 轴系是一种几何元素，表示三维空间中的一组正交坐标轴方向。
///          所有具体轴系类型（地固系、惯性系、冻结系等）均派生自此类。
///          轴系通过父子关系构成树形层级，根轴系通常为惯性空间（如 ICRF）。
class AST_CORE_API Axes : public ObjectNamed
{
public:
    AST_OBJECT(Axes)
    ~Axes() override = default;
    Axes() = default;
    
    A_DISABLE_COPY(Axes)
    
    /// @brief 获取当前轴系的父轴系
    /// @return 父轴系指针，根轴系返回 nullptr
    virtual Axes* getParent() const = 0;

    /// @brief 获取当前轴系相对于父轴系的旋转变换
    /// @param tp 时间点
    /// @param rotation 输出参数，旋转变换
    /// @return 错误码
    virtual errc_t getTransform(const TimePoint& tp, Rotation& rotation) const = 0;
    
    /// @brief 获取当前轴系相对于父轴系的运动学旋转变换
    /// @param tp 时间点
    /// @param rotation 输出参数，运动学旋转变换（包含角速度）
    /// @return 错误码
    virtual errc_t getTransform(const TimePoint& tp, KinematicRotation& rotation) const = 0;
public:
    /// @brief 获取当前轴系的深度
    /// @details 根轴系深度为 0，每增加一级父轴系深度加 1。
    /// @return 深度值
    int getDepth() const;

    /// @brief 获取当前轴系指定深度的祖先轴系
    /// @param depth 目标祖先的深度
    /// @return 祖先轴系指针，若 depth 超出范围则返回 nullptr
    Axes* getAncestor(int depth) const;

public:
    /// @brief 获取当前轴系到目标轴系的旋转变换
    /// @param target 目标轴系
    /// @param tp 时间点
    /// @param rotation 输出参数，旋转变换
    /// @return 错误码
    A_ALWAYS_INLINE
    errc_t getTransformTo(Axes* target, const TimePoint& tp, Rotation& rotation) const
    {
        return aAxesTransform(const_cast<Axes*>(this), target, tp, rotation);
    }
    /// @brief 获取当前轴系到目标轴系的运动学旋转变换
    /// @param target 目标轴系
    /// @param tp 时间点
    /// @param rotation 输出参数，运动学旋转变换（包含角速度）
    /// @return 错误码
    A_ALWAYS_INLINE
    errc_t getTransformTo(Axes* target, const TimePoint& tp, KinematicRotation& rotation) const
    {
        return aAxesTransform(const_cast<Axes*>(this), target, tp, rotation);
    }
    /// @brief 获取源轴系到当前轴系的旋转变换
    /// @param source 源轴系
    /// @param tp 时间点
    /// @param rotation 输出参数，旋转变换
    /// @return 错误码
    A_ALWAYS_INLINE
    errc_t getTransformFrom(Axes* source, const TimePoint& tp, Rotation& rotation) const
    {
        return aAxesTransform(source, const_cast<Axes*>(this), tp, rotation);
    }
    /// @brief 获取源轴系到当前轴系的运动学旋转变换
    /// @param source 源轴系
    /// @param tp 时间点
    /// @param rotation 输出参数，运动学旋转变换（包含角速度）
    /// @return 错误码
    A_ALWAYS_INLINE
    errc_t getTransformFrom(Axes* source, const TimePoint& tp, KinematicRotation& rotation) const
    {
        return aAxesTransform(source, const_cast<Axes*>(this), tp, rotation);
    }
    /// @brief 获取当前轴系相对于父轴系的旋转变换
    /// @param tp 时间点
    /// @param rotation 输出参数，旋转变换
    /// @return 错误码
    A_ALWAYS_INLINE
    errc_t getTransformFromParent(const TimePoint& tp, Rotation& rotation) const
    {
        return getTransform(tp, rotation);
    }
    /// @brief 获取当前轴系相对于父轴系的运动学旋转变换
    /// @param tp 时间点
    /// @param rotation 输出参数，运动学旋转变换（包含角速度）
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
