///
/// @file      SpatialCondCalculationBounds.hpp
/// @brief     计算边界空间条件
/// @author    axel
/// @date      2026-07-29
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

#include "SpatialCondition.hpp"
#include "AstCore/WeakPtr.hpp"

AST_NAMESPACE_BEGIN

class SpatialCalculation;

/// @brief 边界类型
enum class EBoundType
{
    eAboveMinimum,                ///< 高于最小值
    eBelowMaximum,                ///< 低于最大值
    eBetweenMinimumAndMaximum,    ///< 介于最小值和最大值之间
    eOutsideMinimumAndMaximum     ///< 在最小值和最大值之外
};

/// @brief 计算边界空间条件
/// @details 对空间计算的值施加数值边界，值在边界内时条件满足
/// @ingroup Spatial
class AST_CORE_API SpatialCondCalculationBounds final : public SpatialCondition
{
public:
    SpatialCondCalculationBounds() = default;
    ~SpatialCondCalculationBounds() override = default;

    void accept(SpatialConditionVisitor& visitor) override { visitor.visit(*this); }
    errc_t evaluate(const TimePoint& time, const Vector3d& position, bool& result) const override;

    /// @name 引用的空间计算
    /// @{
    void setCalculation(SpatialCalculation* calc);
    SpatialCalculation* getCalculation() const { return calc_.get(); }
    /// @}

    /// @name 边界类型
    /// @{
    void setBoundType(EBoundType type) { boundType_ = type; }
    EBoundType getBoundType() const { return boundType_; }
    /// @}

    /// @name 边界值
    /// @{
    void setLowerBound(double bound) { lowerBound_ = bound; }
    double getLowerBound() const { return lowerBound_; }
    void setUpperBound(double bound) { upperBound_ = bound; }
    double getUpperBound() const { return upperBound_; }
    /// @}

private:
    WeakPtr<SpatialCalculation> calc_;                                     ///< 被引用的空间计算
    EBoundType                   boundType_{EBoundType::eAboveMinimum};     ///< 边界类型
    double                       lowerBound_{0.0};                          ///< 下界
    double                       upperBound_{0.0};                          ///< 上界
};

AST_NAMESPACE_END
