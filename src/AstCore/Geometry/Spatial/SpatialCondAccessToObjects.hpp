///
/// @file      SpatialCondAccessToObjects.hpp
/// @brief     物体可见性空间条件
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
#include "AstCore/BodyPosition.hpp"

#include "DifferentiationSettings.hpp"

AST_NAMESPACE_BEGIN

class Object;

/// @brief 物体可见性空间条件
/// @details 评估父对象与第二对象之间是否存在可见性（Access）
/// @ingroup Spatial
class AST_CORE_API SpatialCondAccessToObjects final : public SpatialCondition
{
public:
    SpatialCondAccessToObjects() = default;
    ~SpatialCondAccessToObjects() override = default;

    void accept(SpatialConditionVisitor& visitor) override { visitor.visit(*this); }
    errc_t evaluate(const TimePoint& time, const Vector3d& position, bool& result) const override;

    /// @name 第二对象
    /// @{
    void setSecondObject(Object* obj);
    Object* getSecondObject() const { return secondObject_.get(); }
    /// @}

    /// @name 光时延迟
    /// @{
    void setApplyLTDelay(bool apply) { applyLTDelay_ = apply; }
    bool getApplyLTDelay() const { return applyLTDelay_; }
    /// @}

    /// @name 时钟主机
    /// @{
    void setClockHost(EClockHost host) { clockHost_ = host; }
    EClockHost getClockHost() const { return clockHost_; }
    /// @}

    /// @name 时间方向
    /// @{
    void setTimeSense(ETimeSense sense) { timeSense_ = sense; }
    ETimeSense getTimeSense() const { return timeSense_; }
    /// @}

    /// @name 光行差类型
    /// @{
    void setAberrationType(EAberrationType type) { aberrationType_ = type; }
    EAberrationType getAberrationType() const { return aberrationType_; }
    /// @}

    /// @name 导数设置
    /// @{
    void setDifferentiation(const DifferentiationSettings& ds) { diffSettings_ = ds; }
    const DifferentiationSettings& getDifferentiation() const { return diffSettings_; }
    /// @}

private:
    WeakPtr<Object>          secondObject_;                          ///< 第二对象（如 Facility/Facility1）
    bool                     applyLTDelay_{true};                    ///< 是否应用光时延迟
    EClockHost               clockHost_{EClockHost::eFirstObject};   ///< 时钟参考
    ETimeSense               timeSense_{ETimeSense::eTransmit};      ///< 时间方向
    EAberrationType          aberrationType_{EAberrationType::eAnnual}; ///< 光行差
    DifferentiationSettings  diffSettings_;                          ///< 导数设置
};

AST_NAMESPACE_END
