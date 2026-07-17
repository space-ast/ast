///
/// @file      VectorCross.cpp
/// @brief     叉乘向量类实现
/// @details   计算两个输入向量叉积的实现。
///            位置: C = A × B
///            速度: dC/dt = (dA/dt × B) + (A × dB/dt)
/// @author    YᴗYang
/// @date      2026-07-16
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

#include "VectorCross.hpp"

AST_NAMESPACE_BEGIN

Axes* VectorCross::getAxes() const
{
    if (vector1_)
    {
        return vector1_->getAxes();
    }
    return nullptr;
}

void VectorCross::setVector1(Vector* vec)
{
    vector1_ = vec;
}

Vector* VectorCross::getVector1() const
{
    return vector1_.get();
}

void VectorCross::setVector2(Vector* vec)
{
    vector2_ = vec;
}

Vector* VectorCross::getVector2() const
{
    return vector2_.get();
}

errc_t VectorCross::getVector(const TimePoint& tp, Vector3d& vec) const
{
    if (!vector1_ || !vector2_)
    {
        return eErrorNullInput;
    }

    // 通过 getVectorIn 将两个输入向量统一到当前叉乘向量声明的坐标系中。
    // 即使两个输入向量原本处于不同坐标系，也能正确计算叉积。
    auto axes = this->getAxes();

    Vector3d v1;
    errc_t err = vector1_->getVectorIn(axes, tp, v1);
    if (err != eNoError) return err;

    Vector3d v2;
    err = vector2_->getVectorIn(axes, tp, v2);
    if (err != eNoError) return err;

    // C = A × B
    vec = v1.cross(v2);
    return eNoError;
}

errc_t VectorCross::getVector(const TimePoint& tp, Vector3d& vec, Vector3d& vel) const
{
    if (!vector1_ || !vector2_)
    {
        return eErrorNullInput;
    }

    // 通过 getVectorIn 将两个输入向量统一到当前叉乘向量声明的坐标系中，
    // 包括值及变化率，保证坐标系一致性。
    auto axes = this->getAxes();

    Vector3d v1, v1_dot;
    errc_t err = vector1_->getVectorIn(axes, tp, v1, v1_dot);
    if (err != eNoError) return err;

    Vector3d v2, v2_dot;
    err = vector2_->getVectorIn(axes, tp, v2, v2_dot);
    if (err != eNoError) return err;

    // C = A × B
    vec = v1.cross(v2);
    // dC/dt = (dA/dt × B) + (A × dB/dt)
    vel = v1_dot.cross(v2) + v1.cross(v2_dot);

    return eNoError;
}

AST_NAMESPACE_END
