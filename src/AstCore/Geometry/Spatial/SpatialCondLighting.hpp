///
/// @file      SpatialCondLighting.hpp
/// @brief     光照空间条件
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
#include "AstCore/SharedPtr.hpp"
#include "AstCore/CelestialBody.hpp"
#include "AstCore/Eclipse.hpp"      // for ELightingType

#include <vector>

AST_NAMESPACE_BEGIN

/// @brief 光照空间条件
/// @details 评估指定位置的光照状态（考虑遮挡天体）
/// @ingroup Spatial
class AST_CORE_API SpatialCondLighting final : public SpatialCondition
{
public:
    SpatialCondLighting() = default;
    ~SpatialCondLighting() override = default;

    void accept(SpatialConditionVisitor& visitor) override { visitor.visit(*this); }
    errc_t evaluate(const TimePoint& time, const Vector3d& position, bool& result) const override;

    /// @name 光照类型
    /// @{
    void setLightingType(ELightingType type) { lightingType_ = type; }
    ELightingType getLightingType() const { return lightingType_; }
    /// @}

    /// @name 遮挡天体
    /// @{
    void addEclipsingBody(const SharedPtr<CelestialBody>& body) { eclipsingBodies_.push_back(body); }
    size_t getEclipsingBodyCount() const { return eclipsingBodies_.size(); }
    const std::vector<SharedPtr<CelestialBody>>& getEclipsingBodies() const { return eclipsingBodies_; }
    /// @}

private:
    ELightingType                           lightingType_{ELightingType::eSunlight};  ///< 光照类型
    std::vector<SharedPtr<CelestialBody>>   eclipsingBodies_;                         ///< 遮挡天体列表
};

AST_NAMESPACE_END
