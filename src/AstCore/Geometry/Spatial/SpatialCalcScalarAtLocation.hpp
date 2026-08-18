///
/// @file      SpatialCalcScalarAtLocation.hpp
/// @brief     位置标量委托计算
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

/// @brief 位置标量委托计算
/// @details 委托给另一个（非空间）标量计算，将父对象放置到网格位置后评估
/// @ingroup Spatial
class AST_CORE_API SpatialCalcScalarAtLocation final : public SpatialCalculation
{
public:
    SpatialCalcScalarAtLocation() = default;
    ~SpatialCalcScalarAtLocation() override = default;

    void accept(SpatialCalculationVisitor& visitor) override { visitor.visit(*this); }
    errc_t evaluate(const TimePoint& time, const Vector3d& position, double& result) const override;

    /// @name 被引用的计算
    /// @{
    void setReferencedCalculation(SpatialCalculation* calc);
    SpatialCalculation* getReferencedCalculation() const { return refCalc_.get(); }
    /// @}

private:
    WeakPtr<SpatialCalculation> refCalc_;  ///< 被引用的（非空间）标量计算
};

AST_NAMESPACE_END
