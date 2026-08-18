///
/// @file      SpatialCalcSatisfactionMetrics.hpp
/// @brief     空间条件满足度量计算
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

#include "SpatialCalculation.hpp"
#include "AstCore/WeakPtr.hpp"

AST_NAMESPACE_BEGIN

class SpatialCondition;

/// @brief 满足度量类型
enum class ESatisfactionMetric
{
    eIntervalDuration,         ///< 满足区间时长
    eGapDuration,              ///< 间隔时长
    eNumberOfIntervals,        ///< 满足区间数量
    eNumberOfGaps,             ///< 间隔数量
    eTimeSinceLastSatisfaction, ///< 距上次满足的时间
    eTimeUntilNextSatisfaction  ///< 距下次满足的时间
};

/// @brief 累积类型
enum class EAccumulationType
{
    eTotal,              ///< 整个分析区间累积
    eCurrent,            ///< 瞬时值
    eUpToCurrentTime,    ///< 累积到当前时间
    eFromCurrentTime     ///< 从当前时间开始累积
};

/// @brief 区间过滤类型
enum class EIntervalFilter
{
    eNone,              ///< 不过滤
    eFirstInterval,     ///< 仅第一个区间
    eLastInterval,      ///< 仅最后一个区间
    eDurationGreaterThan, ///< 时长大于指定值
    eDurationLessThan    ///< 时长小于指定值
};

/// @brief 空间条件满足度量计算
/// @details 从空间条件的满足区间派生标量度量
/// @ingroup Spatial
class AST_CORE_API SpatialCalcSatisfactionMetrics final : public SpatialCalculation
{
public:
    SpatialCalcSatisfactionMetrics() = default;
    ~SpatialCalcSatisfactionMetrics() override = default;

    void accept(SpatialCalculationVisitor& visitor) override { visitor.visit(*this); }
    errc_t evaluate(const TimePoint& time, const Vector3d& position, double& result) const override;

    /// @name 空间条件引用
    /// @{
    void setCondition(SpatialCondition* condition);
    SpatialCondition* getCondition() const { return condition_.get(); }
    /// @}

    /// @name 度量类型
    /// @{
    void setSatisfactionMetric(ESatisfactionMetric metric) { metricType_ = metric; }
    ESatisfactionMetric getSatisfactionMetric() const { return metricType_; }
    /// @}

    /// @name 累积类型
    /// @{
    void setAccumulationType(EAccumulationType accumulation) { accumulationType_ = accumulation; }
    EAccumulationType getAccumulationType() const { return accumulationType_; }
    /// @}

    /// @name 过滤
    /// @{
    void setIntervalFilter(EIntervalFilter filter) { filter_ = filter; }
    EIntervalFilter getIntervalFilter() const { return filter_; }
    /// @}

private:
    WeakPtr<SpatialCondition> condition_;                                         ///< 被引用的空间条件
    ESatisfactionMetric       metricType_{ESatisfactionMetric::eIntervalDuration}; ///< 度量类型
    EAccumulationType         accumulationType_{EAccumulationType::eTotal};        ///< 累积方式
    EIntervalFilter           filter_{EIntervalFilter::eNone};                     ///< 区间过滤
};

AST_NAMESPACE_END
