///
/// @file      DetectorPeriapsis.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-05-03
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

#include "DetectorPeriapsis.hpp"
#include "AstUtil/Logger.hpp"
#include "AstCore/SpacecraftState.hpp"
#include "AstCore/OrbitElement.hpp"


AST_NAMESPACE_BEGIN

DetectorPeriapsis* DetectorPeriapsis::New()
{
    return new DetectorPeriapsis();
}

DetectorPeriapsis::DetectorPeriapsis()
{
    this->setDirection(EDirection::eIncrease);
}

double DetectorPeriapsis::getValue(const SpacecraftState& scState, double t) const
{
    auto body = this->body();
    A_UNUSED(body);
    auto state = scState.getOrbitState();
    // @todo: 处理其他天体的情况
    CartState cartState;
    errc_t rc = state->getState(cartState);
    if(rc)
        aWarning("failed to get state");
    double v = cartState.vel().dot(cartState.pos().normalized());    
    return v;
}

AST_NAMESPACE_END
