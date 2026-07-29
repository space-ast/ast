///
/// @file      SpatialCondOverTime.hpp
/// @brief     时间段空间条件
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

/// @brief 时长类型
enum class EDurationType
{
    eStatic,               ///< 静态（使用原始条件的瞬时评估）
    eCumulativeToCurrent,  ///< 累积到当前时间
    eCumulativeFromCurrent, ///< 从当前时间开始累积
    eSlidingWindow         ///< 滑动窗口
};

/// @brief 时间段空间条件
/// @details 在指定时间区间内评估另一个空间条件，只要在区间内曾满足即视为满足
/// @ingroup Spatial
class AST_CORE_API SpatialCondOverTime final : public SpatialCondition
{
public:
    SpatialCondOverTime() = default;
    ~SpatialCondOverTime() override = default;

    void accept(SpatialConditionVisitor& visitor) override { visitor.visit(*this); }
    errc_t evaluate(const TimePoint& time, const Vector3d& position, bool& result) const override;

    /// @name 原始空间条件
    /// @{
    void setOriginalCondition(SpatialCondition* condition);
    SpatialCondition* getOriginalCondition() const { return origCondition_.get(); }
    /// @}

    /// @name 时长类型
    /// @{
    void setDurationType(EDurationType type) { durationType_ = type; }
    EDurationType getDurationType() const { return durationType_; }
    /// @}

private:
    WeakPtr<SpatialCondition> origCondition_;                 ///< 原始空间条件
    EDurationType              durationType_{EDurationType::eStatic};  ///< 时长处理方式
};

AST_NAMESPACE_END
