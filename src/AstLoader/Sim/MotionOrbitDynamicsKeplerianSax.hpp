///
/// @file      TwoBodySAX.hpp
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

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

class AST_LOADER_API MotionOrbitDynamicsKeplerianSax: public MotionOrbitDynamicsSax
{
public:
    using MotionOrbitDynamicsSax::MotionOrbitDynamicsSax;
    ~MotionOrbitDynamicsKeplerianSax() override = default;
public:
    errc_t keyValue(StringView key, const ValueView& value) override;
    errc_t getMotion(ScopedPtr<MotionProfile>& motion) override;
    virtual MotionOrbitDynamics* newMotionOrbitDynamics() = 0;
protected:
    double radiusOfPerigee_ = 0.0;
    double inclination_ = 0.0;
    double eccentricity_ = 0.0;
    double argOfPerigee_ = 0.0;
    double rightAscension_ = 0.0;
    double trueAnomaly_ = 0.0;
    double timeStep_ = 0.0;
    std::string orbElemCoordSys_{};
    SharedPtr<Axes> orbElemCoordAxes_{};
    std::string propagationCoordSys_{};
    SharedPtr<Axes> propagationCoordAxes_{};
    int displayCoordType_ = 0;
    std::string displayCoordSys_{};
    SharedPtr<Axes> displayCoordAxes_{};
};


/*! @} */

AST_NAMESPACE_END
