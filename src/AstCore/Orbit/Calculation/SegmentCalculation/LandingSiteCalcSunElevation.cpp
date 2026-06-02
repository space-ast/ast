///
/// @file      LandingSiteCalcSunElevation.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-06-02
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

#include "LandingSiteCalcSunElevation.hpp"
#include "AstCore/LandingSite.hpp"
#include "AstCore/CelestialBody.hpp"
#include "AstCore/BodyShape.hpp"
#include "AstCore/BodyPosition.hpp"
#include "AstCore/AER.hpp"
#include "AstUtil/Logger.hpp"

AST_NAMESPACE_BEGIN

errc_t LandingSiteCalcSunElevation::calculate(const Segment& segment, double& result)
{
    const auto* landingSite = aobject_cast<const LandingSite*>(&segment);
    if(!landingSite)
    {
        aError("segment type is not 'LandingSite'");
        return eErrorInvalidType;
    }
    auto landingBody = landingSite->body();
    const auto& position = landingSite->position();  // 着陆点位置
    const auto& epoch = landingSite->epoch();        // 着陆点时间
    
    // 计算太阳相对于着陆点的AER
    AER aer;
    errc_t rc = aSunAERInTopocentric(epoch, landingBody, position, aer);
    result = aer.elevation();
    return rc;
}


AST_NAMESPACE_END

