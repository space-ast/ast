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

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

class MotionHPOP;
using PMotionHPOP = MotionHPOP*;
using HMotionHPOP = SharedPtr<MotionHPOP>;

class AST_SIM_API MotionHPOP: public MotionBasic
{
public:
    static PMotionHPOP New();
    MotionHPOP() = default;
    ~MotionHPOP() override = default;
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

protected:
    HPOPForceModel forceModel_;     ///< 力模型
};

/*! @} */

AST_NAMESPACE_END