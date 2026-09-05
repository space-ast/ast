///
/// @file      DetectorRMagnitude.cpp
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

#include "DetectorRMagnitude.hpp"
#include "AstUtil/Logger.hpp"
#include "AstCore/SpacecraftState.hpp"
#include "AstCore/OrbitElement.hpp"
#include "AstCore/Point.hpp"

AST_NAMESPACE_BEGIN

double DetectorRMagnitude::getValue(const SpacecraftState& state, double t) const
{
    constexpr double nan = std::numeric_limits<double>::quiet_NaN();
    errc_t rc;
    auto point = this->point();
    if(!point) return nan;
    auto frame = state.getFrame();
    TimePoint time{};
    rc = state.getStateEpoch(time);
    if(rc){
        aWarning("failed to get state epoch");
        return nan;
    }
    Vector3d pos;
    rc = point->getPosIn(frame, time, pos);
    if(rc){
        aWarning("failed to get position");
        return nan;
    }
    CartState cartState;
    rc = state.getState(cartState);
    if(rc){
        aWarning("failed to get state");
        return nan;
    }
    double r = (cartState.pos() - pos).norm();
    return r;
}

AST_NAMESPACE_END
