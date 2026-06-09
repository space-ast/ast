///
/// @file      MotionHPOPSax.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-19
/// @copyright 版权所有 (C) 2026-present, ast项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
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
#include "MotionOrbitDynamicsSax.hpp"
#include "AstCore/StateCartesian.hpp"
#include "AstSim/MotionHPOP.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

class MotionHPOPSax : public MotionOrbitDynamicsSax
{
public:
    using MotionOrbitDynamicsSax::MotionOrbitDynamicsSax;
    ~MotionHPOPSax() override = default;
public:
    errc_t keyValue(StringView key, const ValueView& value) override;
    errc_t getMotion(ScopedPtr<MotionProfile>& motion) override;public:
public:
protected:
    double getMass() const {return spacecraftParam_.mass_;}
private:
    CartState cartState_{};                ///< 直角坐标
    HPOPForceModel forceModel_{};          ///< 力模型
    struct {
        double mass_{1000};
        double dragCoefficient_{0.0};
        double areaMassRatio_{0.0};
    } spacecraftParam_;
    struct {
        std::string method_;               ///< 积分器方法 (IntegMethod)
        std::string stepControlMethod_;    ///< 步长控制方法 (StepControlMethod)
        double errorTolerance_ = 1e-13;    ///< 误差容限 (ErrorTolerance)
        double timeStep_ = 60.0;           ///< 时间步长 (TimeStep)
        double minStepSize_ = 1.0;         ///< 最小步长 (MinStepSize)
        double maxStepSize_ = 86400.0;     ///< 最大步长 (MaxStepSize)
        bool reportOnFixedStep_ = true;    ///< 按固定步长报告 (ReportOnFixedStep)
        int interpolationSamplesM1_ = 7;   ///< 插值采样数-1 (InterpolationSamplesM1)
        std::string interpolationMethod_;  ///< 插值方法 (InterpolationMethod)
        double altitudeCutOff_ = 10000.0;  ///< 高度截止 (AltitudeCutOff)
    } integrator_;                      ///< 积分器配置参数
};

/*! @} */

AST_NAMESPACE_END
