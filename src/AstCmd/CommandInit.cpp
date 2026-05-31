///
/// @file      CommandInit.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-05-11
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

#include "CommandInit.hpp"
#include "CommandDispatcher.hpp"
#include "AstUtil/Logger.hpp"
#include "AstUtil/IO.hpp"
#include "AstSim/Mover.hpp"
#include "AstSim/MotionJ2Analytical.hpp"
#include "AstSim/MotionJ4Analytical.hpp"
#include "AstSim/MotionTwoBody.hpp"
#include "AstCore/StateCartesian.hpp"
#include "AstCore/StateKeplerian.hpp"
#include "AstCore/Resolve.hpp"
#include "AstCore/CelestialBody.hpp"
#include <stdexcept>
#include <exception>

AST_NAMESPACE_BEGIN


MotionOrbitDynamics* aNewPropagator(StringView propagator)
{
    if(propagator == "J2Perturbation")
        return new MotionJ2Analytical();
    else if(propagator == "J4Perturbation")
        return new MotionJ4Analytical();
    else if(propagator == "TwoBody")
        return new MotionTwoBody();
    else
        return nullptr;
}

Frame* aResolveFrame(CelestialBody* body, StringView coordSys)
{
    Frame* frame = aResolveFrame(coordSys);
    if(!frame)
    {
        frame = aObject_GetFrame(body, coordSys);
    }
    return frame;
}


void aCommandInitMover(CommandDispatcher& dispatcher)
{
    COMMAND_RULE(dispatcher,
        "SetState <object> Cartesian <string> <string> <string> "
        "<double> <string> <string> "
        "<double> <double> <double> <double> <double> <double>"
    )
    (
        [](Object* obj, StringView propagator, StringView start, StringView stop,
        double stepSize, StringView coordSys, StringView epoch,
        double x, double y, double z, double vx, double vy, double vz) 
        {
            Mover* mover = aobject_cast<Mover*>(obj);
            if(!mover)
                throw std::runtime_error("failed to find object as Mover");
            Body* body = mover->getBody();
            if(!body)
                body = aGetEarth();
            SharedPtr<MotionOrbitDynamics> motion = aNewPropagator(propagator);
            if(!motion)
                throw std::runtime_error(std::string("unsupported propagator: ") + std::string(propagator));
            motion->setInterval(TimeInterval::Parse(start, stop));
            motion->setStepSize(stepSize);
            SharedPtr<StateCartesian> state = aNewObject<StateCartesian>();
            Frame* frame = aResolveFrame(body, coordSys);
            if(!frame)
                throw std::runtime_error(std::string("unsupported frame: ") + std::string(coordSys));
            state->setFrame(frame);
            state->setStateEpoch(TimePoint::Parse(epoch));
            state->setX(x);
            state->setY(y);
            state->setZ(z);
            state->setVx(vx);
            state->setVy(vy);
            state->setVz(vz);
            motion->setInitialState(state);
            motion->setPropagationFrame(body->makeFrameInertial());
            mover->setMotionProfile(motion);
            return 0;
        }
    );

    COMMAND_RULE(dispatcher,
        "SetState <object> Classical <string> <string> <string> "
        "<double> <string> <string> "
        "<double> <double> <double> <double> <double> <double>"
    )
    (
        [](Object* obj, StringView propagator, StringView start, StringView stop,
        double stepSize, StringView coordSys, StringView epoch,
        double a, double ecc, double inc, double arg, double raan, double M) 
        {
            Mover* mover = aobject_cast<Mover*>(obj);
            if(!mover)
                throw std::runtime_error("failed to find object as Mover");
            Body* body = mover->getBody();
            if(!body)
                body = aGetEarth();

            SharedPtr<MotionOrbitDynamics> motion = aNewPropagator(propagator);
            if(!motion)
                throw std::runtime_error(std::string("unsupported propagator: ") + std::string(propagator));
            motion->setInterval(TimeInterval::Parse(start, stop));
            motion->setStepSize(stepSize);
            SharedPtr<StateKeplerian> state = aNewObject<StateKeplerian>();
            Frame* frame = aResolveFrame(body, coordSys);
            if(!frame)
                throw std::runtime_error(std::string("unsupported frame: ") + std::string(coordSys));
            state->setFrame(frame);
            state->setStateEpoch(TimePoint::Parse(epoch));
            state->setSMA(a);
            state->setEcc(ecc);
            state->setInc(inc);
            state->setArgPeri(arg);
            state->setRAAN(raan);
            state->setMeanAnomaly(M);
            motion->setInitialState(state);
            motion->setPropagationFrame(body->makeFrameInertial());
            mover->setMotionProfile(motion);
            return 0;
        }
    );

}

void aCommandInitBasic(CommandDispatcher& dispatcher)
{
    COMMAND_RULE(dispatcher,
        "Test1 <string> <string>"
    )
    ([](StringView scenario, StringView filePath) {
            ast_printf("Test1: Scenario = '%.*s', FilePath = '%.*s'\n", 
                (int)scenario.size(), scenario.data(), 
                (int)filePath.size(), filePath.data());
            return 0;
        }
    );

    COMMAND_RULE(dispatcher,
        "Test2 <string> <string>"
    )
    ([](StringView appPath, StringView unit) {
        ast_printf("Test2: Application = '%.*s', Unit = '%.*s'\n", 
            (int)appPath.size(), appPath.data(), 
            (int)unit.size(), unit.data());
        return 0;
    });

    COMMAND_RULE(dispatcher,
        "Test3 <string> <string>"
    )
    ([](StringView objectPath, StringView style) {
        ast_printf("Test3: Object = '%.*s', Style = '%.*s'\n", 
            (int)objectPath.size(), objectPath.data(), 
            (int)style.size(), style.data());
        return 0;
    });

    COMMAND_RULE(dispatcher,
        "DoesObjExist <string> <object>"
    )
    ([](StringView applicationPath, Object* object) {
        if(object)
            return "1";
        else
            return "0";
    });

    COMMAND_RULE(dispatcher,
        "New <string> <string> <string>"
    )
    ([](StringView applicationPath, StringView classPath, StringView newObjectName){
        auto pos = classPath.find_last_of('/');
        if(pos == std::string::npos)
        {
            Object* obj = aNewObject(classPath);
            if(obj){
                aAddObject(obj);
                obj->setName(newObjectName);
                return 0;
            }
            throw std::runtime_error(std::string("failed to create object: ") + std::string(classPath));
        }
        StringView className = classPath.substr(pos + 1);
        StringView parentObjectPath = classPath.substr(0, pos);
        Object* parentObject = aResolveObject(parentObjectPath);
        if(!parentObject)
            throw std::runtime_error(std::string("failed to find parent object: ") + std::string(parentObjectPath));
        Object* newObject = aNewObject(className, parentObject);
        if(!newObject)
            throw std::runtime_error(std::string("failed to create object: ") + std::string(className));
        newObject->setName(newObjectName);
        aAddObject(newObject);
        return 0;
    });
}

/// 兼容Astrogator命令
void aCommandInitAstrogatorComp(CommandDispatcher& dispatcher)
{
    COMMAND_RULE(dispatcher,
        "Astrogator <object> ApplyScript <string:Attribute>"
    )
    ([](Object* obj, StringView attribute) {
        if(!obj)
            throw std::runtime_error("failed to find object");
        if(!attribute.empty())
            throw std::runtime_error("failed to find attribute");
        return 0;
    }
    );
}


void aCommandDispatcherInit(CommandDispatcher& dispatcher)
{
    aCommandInitBasic(dispatcher);
    aCommandInitMover(dispatcher);
    aCommandInitAstrogatorComp(dispatcher); // 兼容Astrogator命令
}

AST_NAMESPACE_END


