///
/// @file      MotionHPOP.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-14
/// @copyright 版权所有 (C) 2026-present, ast项目.
///
/// ast项目（https://github.com/space-ast/ast）
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
#include "AstSim/MotionOrbitDynamics.hpp"
#include "AstCore/HPOPForceModel.hpp"
#include "AstCore/SpacecraftParam.hpp"
#include "AstMath/ODEIntegrator.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

class MotionHPOP;
class ODEIntegrator;
using PMotionHPOP = MotionHPOP*;
using HMotionHPOP = SharedPtr<MotionHPOP>;

class AST_SIM_API MotionHPOP: public MotionBasic
{
public:
    static PMotionHPOP New();
    MotionHPOP();
    ~MotionHPOP() override;
public:
    errc_t makeEphemerisSpec(ScopedPtr<Ephemeris>& eph) const override;
    errc_t makeEphemerisSimple(ScopedPtr<Ephemeris>& eph) const override;
    void accept(MotionProfileVisitor& visitor) override;
public:
    /// @brief 设置力模型
    /// @param forceModel 力模型
    void setForceModel(HPOPForceModel&& forceModel){forceModel_ = std::move(forceModel);}
    void setForceModel(const HPOPForceModel& forceModel){forceModel_ = forceModel;}

    /// @brief 获取力模型
    /// @return 力模型
    const HPOPForceModel& getForceModel() const{return forceModel_;}

    /// @brief 设置航天器参数
    void setSpacecraftParam(const SpacecraftParam& spacecraftParam){spacecraftParam_= spacecraftParam;}

    /// @brief 获取航天器参数
    const SpacecraftParam& getSpacecraftParam() const{return spacecraftParam_;}

    /// @brief 设置积分器
    /// @param integrator 积分器
    void setIntegrator(ODEIntegrator* integrator);

    /// @brief 获取积分器
    /// @return 积分器
    ODEIntegrator* getIntegrator() const;
private:
    HPOPForceModel forceModel_{};            ///< 力模型
    SpacecraftParam spacecraftParam_{};      ///< 航天器参数
    SharedPtr<ODEIntegrator> integrator_{};  ///< 积分器
};

/*! @} */

AST_NAMESPACE_END