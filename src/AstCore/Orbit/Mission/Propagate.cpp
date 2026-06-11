///
/// @file      Propagate.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-04-15
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

#include "Propagate.hpp"
#include "AstCore/HPOP.hpp"
#include "AstUtil/Logger.hpp"
#include "AstCore/SpacecraftState.hpp"
#include "AstCore/OrbitElement.hpp"
#include "AstMath/ODEIntegrator.hpp"
#include "AstUtil/RTTIAPI.hpp"

AST_NAMESPACE_BEGIN

#define _AST_DEBUG_PROPAGATE

Propagate::Propagate()
{
    propagator_ = aNewObject<HPOP>(this, "Propagator");
}

errc_t Propagate::execute()
{
    const auto inputState   = this->getInputState();  AST_CHECK_NULLPTR(inputState);
    auto outputState  = this->getOutputState();       AST_CHECK_NULLPTR(outputState);
    auto propagator   = this->propagator();           AST_CHECK_NULLPTR(propagator);
    auto integrator   = propagator->getIntegrator();  AST_CHECK_NULLPTR(integrator);
    Frame* propFrame = propagator->propagationFrame();  AST_CHECK_NULLPTR(propFrame);
    
    // 获取初始状态
    State* orbitState = inputState->getOrbitState();  AST_CHECK_NULLPTR(orbitState);
    TimePoint startTime;
    errc_t rc = orbitState->getStateEpoch(startTime); AST_CHECK_ERRCODE(rc, "Failed to get state epoch");
    CartState inputCartState;
    rc = orbitState->getStateIn(propFrame, inputCartState);             AST_CHECK_ERRCODE(rc, "Failed to get cart state");
    
    TimePoint endTime;
    if(direction_ == eBackward){
        endTime = startTime - maxPropTime();
    }
    else // if(direction_ == eForward)
    {
        endTime = startTime + maxPropTime();
    }
    // 添加停止条件
    integrator->clearEventDetectors();
    for(auto& eventDetector: eventDetectors_)
    {
        // 检查事件检测器是否激活
        if(eventDetector && eventDetector->active())
            propagator->addEventDetector(eventDetector);
    }
    // 设置航天器参数
    propagator->setSpacecraftParam(inputState->spacecraftParam());
    // 进行轨道预报
    CartState outputCartState = inputCartState;
    rc = propagator->propagate(startTime, endTime, outputCartState.pos(), outputCartState.vel()); AST_CHECK_ERRCODE(rc, "Failed to propagate");
    // 输出结果
    outputState->copyFrom(*inputState);
    outputState->setStateEpoch(endTime);
    outputState->setFrame(propFrame);
    rc = outputState->setState(outputCartState);       AST_CHECK_ERRCODE(rc, "Failed to set cart state");

    #ifdef _AST_DEBUG_PROPAGATE
    printf("\n------------------------------------\n");
    printf("Propagate: %s\n", getName().c_str());
    printf("------------------------------------\n");
    printf("startTime: %s\n", startTime.toString().c_str());
    printf("startState: %s\n", inputCartState.toString().c_str());
    printf("endTime: %s\n", endTime.toString().c_str());
    printf("endState: %s\n", outputCartState.toString().c_str());
    printf("------------------------------------\n");
    #endif
    return 0;
}

EventDetector* Propagate::getEventDetector(StringView name) const
{
    for(auto& eventDetector: eventDetectors_)
    {
        if(name == eventDetector->getName())
            return eventDetector.get();
    }
    return nullptr;
}

AST_NAMESPACE_END
