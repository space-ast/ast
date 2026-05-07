///
/// @file      FrameAssembly.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-11
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
#include "AstCore/Frame.hpp"
#include "AstCore/Axes.hpp"
#include "AstCore/Point.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup Coordinate
    @{
*/

class FrameAssembly;
using HFrameAssembly = SharedPtr<FrameAssembly>;
using PFrameAssembly = FrameAssembly*;

/// @brief 组装坐标系
/// @details 组装坐标系由轴系和原点组成
/// 可以通过轴系和原点的灵活组合定义坐标系
class AST_CORE_API FrameAssembly: public Frame
{
public:
    AST_OBJECT(FrameAssembly)
    AST_PROPERT(Origin)
    AST_PROPERT(Axes)
    /// @brief 创建组装坐标系
    /// @param origin 原点
    /// @param axes 轴系
    /// @warning 返回的指针为裸指针，需要手动管理内存，避免内存泄漏
    /// @return 组装坐标系指针
    static PFrameAssembly New(Point* origin, Axes* axes);

    /// @brief 创建组装坐标系
    /// @param origin 原点
    /// @param axes 轴系
    /// @note 返回的智能指针会自动管理内存
    /// @return 组装坐标系智能指针
    static HFrameAssembly MakeShared(Point* origin, Axes* axes);

    FrameAssembly() = default;
    FrameAssembly(Point* origin, Axes* axes);
    ~FrameAssembly() = default;

    Frame* getParent() const final;
    errc_t getTransform(const TimePoint&tp, Transform& transform) const override;
    errc_t getTransform(const TimePoint&tp, KinematicTransform& transform) const override;
PROPERTIES:
    Point* getOrigin() const final { return origin_.get(); }
    void setOrigin(Point* origin) { origin_ = origin; }
    Axes* getAxes() const final { return axes_.get(); }
    void setAxes(Axes* axes) { axes_ = axes; }
protected:
    SharedPtr<Point> origin_;    ///< 原点
    SharedPtr<Axes>  axes_;      ///< 轴系统
};

/*! @} */

AST_NAMESPACE_END
