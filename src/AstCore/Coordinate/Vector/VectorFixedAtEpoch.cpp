///
/// @file      VectorFixedAtEpoch.cpp
/// @brief     历元固定向量类实现
/// @details   实现Fixed at Epoch向量的位置和速度计算。
///            在历元时刻从源向量捕获其值，通过 getVectorIn 转换到参考轴系，
///            此后值在参考轴系中保持恒定，速度恒为零。
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

#include "VectorFixedAtEpoch.hpp"

AST_NAMESPACE_BEGIN

void VectorFixedAtEpoch::setSourceVector(Vector* vec)
{
    sourceVector_ = vec;
}

Vector* VectorFixedAtEpoch::getSourceVector() const
{
    return sourceVector_.get();
}

void VectorFixedAtEpoch::setAxes(Axes* axes)
{
    referenceAxes_ = axes;
}

Axes* VectorFixedAtEpoch::getAxes() const
{
    return referenceAxes_.get();
}

void VectorFixedAtEpoch::setEpoch(const TimePoint& epoch)
{
    epoch_ = epoch;
}

const TimePoint& VectorFixedAtEpoch::getEpoch() const
{
    return epoch_;
}

errc_t VectorFixedAtEpoch::getVector(const TimePoint& tp, Vector3d& vec) const
{
    (void)tp;  // 捕获后值固定，不随查询时间变化

    if (!sourceVector_ || !referenceAxes_)
    {
        return eErrorNullInput;
    }

    // 在历元时刻从源向量捕获值，并转换到参考轴系中表达
    return sourceVector_->getVectorIn(referenceAxes_.get(), epoch_, vec);
}

errc_t VectorFixedAtEpoch::getVector(const TimePoint& tp, Vector3d& vec, Vector3d& vel) const
{
    (void)tp;  // 捕获后值固定，不随查询时间变化

    if (!sourceVector_ || !referenceAxes_)
    {
        return eErrorNullInput;
    }

    // 在历元时刻从源向量捕获值，转换到参考轴系中表达
    errc_t err = sourceVector_->getVectorIn(referenceAxes_.get(), epoch_, vec);
    if (err != eNoError) return err;

    // 捕获后向量固定于参考轴系，速度为零
    vel = Vector3d::Zero();
    return eNoError;
}

AST_NAMESPACE_END
