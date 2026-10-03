///
/// @file      AttitudeAlignConstrain.hpp
/// @brief     对齐/约束姿态(Aligned and Constrained)
/// @details   STK 中大多数预定义姿态剖面都是由"一对对齐向量 + 一对约束向量"生成的，
///            本文件提供该机制的求解引擎(aAlignConstrainRotation)以及通用剖面
///            AttitudeAlignConstrain，具体剖面(ECIVVLH、ECFVelRadial 等)都建立在其之上。
/// @author    axel
/// @date      2026-09-28
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
#include "AstCore/AttitudeProfile.hpp"
#include "AstCore/Vector.hpp"
#include "AstMath/Rotation.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup Attitude
    @{
*/


/// @brief 对齐/约束姿态(Aligned and Constrained)
/// @details 
/// 用一对"对齐"矢量和一对"约束"矢量来定姿：
/// 体系下的对齐矢量与参考系下的对齐矢量严格对齐，
/// 在此前提下让体系下的约束矢量尽可能接近参考系下的约束矢量
class AST_CORE_API AttitudeAlignConstrain : public AttitudeProfile
{
public:
    AST_OBJECT(AttitudeAlignConstrain)

    AttitudeAlignConstrain() = default;
    ~AttitudeAlignConstrain() override = default;
public:
    Axes* getParent() const override;
    errc_t getTransform(const TimePoint& tp, Rotation& rotation) const override;
    errc_t getTransform(const TimePoint& tp, KinematicRotation& rotation) const override;
public:
    const Vector3d& alignedAxis() const{return alignedAxis_;}
    void setAlignedAxis(const Vector3d& axis){alignedAxis_ = axis;}

    const Vector3d& constrainedAxis() const{return constrainedAxis_;}
    void setConstrainedAxis(const Vector3d& axis){constrainedAxis_ = axis;}
    
    Vector* alignedVector() const{return alignedVector_.get();}
    void setAlignedVector(Vector* vector){alignedVector_ = vector;}
    
    Vector* constrainedVector() const{return constrainedVector_.get();}
    void setConstrainedVector(Vector* vector){constrainedVector_ = vector;}
private:
    Vector3d alignedAxis_{};
    Vector3d constrainedAxis_{};
    WeakPtr<Vector> alignedVector_{};
    WeakPtr<Vector> constrainedVector_{};
};

/*! @} */

AST_NAMESPACE_END
