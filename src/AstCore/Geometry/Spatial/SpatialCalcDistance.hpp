///
/// @file      SpatialCalcDistance.hpp
/// @brief     位置距离空间计算
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

class Object;

/// @brief 距离参考配置
enum class EDistanceRefConfig
{
    eFromPoint,       ///< 到参考点的距离
    eFromPlane,       ///< 到参考平面的距离（无符号）
    eFromPlaneSigned, ///< 到参考平面的距离（带符号）
    eAlongVector,     ///< 沿参考向量的投影距离（无符号）
    eAlongVectorSigned ///< 沿参考向量的投影距离（带符号）
};

/// @brief 位置距离空间计算
/// @details 评估指定位置到参考对象（点/平面/向量）的距离
/// @ingroup Spatial
class AST_CORE_API SpatialCalcDistance final : public SpatialCalculation
{
public:
    SpatialCalcDistance() = default;
    ~SpatialCalcDistance() override = default;

    void accept(SpatialCalculationVisitor& visitor) override { visitor.visit(*this); }
    errc_t evaluate(const TimePoint& time, const Vector3d& position, double& result) const override;

    /// @name 参考对象
    /// @{
    void setReferenceObject(Object* obj);
    Object* getReferenceObject() const { return refObject_.get(); }
    /// @}

    /// @name 参考配置
    /// @{
    void setReferenceConfig(EDistanceRefConfig config) { refConfig_ = config; }
    EDistanceRefConfig getReferenceConfig() const { return refConfig_; }
    /// @}

private:
    WeakPtr<Object>      refObject_;                                    ///< 参考对象（点/平面/向量）
    EDistanceRefConfig   refConfig_{EDistanceRefConfig::eFromPoint};    ///< 距离测量方式
};

AST_NAMESPACE_END
