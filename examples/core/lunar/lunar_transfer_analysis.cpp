///
/// @file      lunar_transfer_analysis.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-09-04
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


#include "ast/HPOP.hpp"
#include "ast/HPOPForceModel.hpp"
#include "ast/Resolve.hpp"
#include "ast/OrbitElement.hpp"
#include "ast/SpacecraftParam.hpp"
#include "ast/Literals.hpp"
#include "ast/DetectorPeriapsis.hpp"

AST_USING_NAMESPACE

HPOPForceModel forceModel()
{
    HPOPForceModel fm;
    
    fm.setCentralBody("Earth"_body);
    fm.gravity().maxOrder_  = 8;
    fm.gravity().maxDegree_ = 8;
    fm.gravity().model_ = "WGS84";
    
    auto moonForce = fm.addThirdBody("Moon"_body);
    moonForce->setAttractionType(EBodyAttractionType::ePointMass);
    moonForce->pointMass().setSpecifiedGM(4902800000000);

    auto sunForce   = fm.addThirdBody("Sun"_body);
    sunForce->setAttractionType(EBodyAttractionType::ePointMass);
    sunForce->pointMass().setSpecifiedGM(1.32712000000000e+20);

    fm.useDrag(false);
    fm.useSRP(false);

    return fm;
}


void analyseLunarTransfer()
{
    HPOPForceModel fm = forceModel();
    CartState initState{6159968.33397138, 2393641.1690744, 922487.447464321, -2872.54217681962, 9257.74498172147, -4877.87739768414};
    TimePoint initTime = "2017-12-31 20:51:50.678"_utc;
    auto state = initState;
    SpacecraftParam spacecraftParam;
    HPOP hpop;
    SharedPtr<DetectorPeriapsis> detector = aMakeShared<DetectorPeriapsis>();
    detector->setBody("Moon"_body);
    hpop.setSpacecraftParam(spacecraftParam);
    hpop.setForceModel(fm);
    hpop.addEventDetector(detector.get());
    TimePoint stopTime = initTime + 10_day;
    errc_t rc = hpop.propagate(initTime, stopTime, state.pos(), state.vel());
    printf("stopTime: %s\n", stopTime.toString().c_str());
    printf("state: %s\n", state.toString().c_str());
}

int main()
{
    analyseLunarTransfer();
}
