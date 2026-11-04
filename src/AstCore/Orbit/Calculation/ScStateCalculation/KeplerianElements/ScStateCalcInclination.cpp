///
/// @file      ScStateCalcInclination.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-05-02
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

#include "ScStateCalcInclination.hpp"
#include "AstCore/OrbitElement.hpp"
#include "AstCore/BuiltinFrame.hpp"
#include "AstCore/CelestialBody.hpp"
#include "AstUtil/Logger.hpp"

AST_NAMESPACE_BEGIN

errc_t ScStateCalcInclination::calculate(const SpacecraftState& state, double& result)
{
    auto frame = this->frame();
    if(frame == nullptr)
    {
        // 未配置参考系时依次回退到地球惯性系、ECI 系
        auto body = aGetEarth();
        if(body)
            frame = body->getFrameInertial();
        if(frame == nullptr)
            frame = aFrameECI();
        if(frame == nullptr)
        {
            aError(_("未配置参考系"));
            return eErrorInvalidParam;
        }
    }
    ModOrbElem orbElem;
    errc_t rc = state.getStateIn(frame, orbElem);
    if(rc != eNoError)
        return rc;
    result = orbElem.getInc();
    return eNoError;
}

AST_NAMESPACE_END