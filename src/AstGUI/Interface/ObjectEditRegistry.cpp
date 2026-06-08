///
/// @file      ObjectEditRegistry.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-04-02
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

#include "ObjectEditRegistry.hpp"

#include "AstUtil/Object.hpp"

#include "AstCore/BodyEphemerisDE.hpp"
#include "AstCore/BodyEphemerisSPK.hpp"
#include "AstCore/BurnImpulsive.hpp"
#include "AstCore/CelestialBody.hpp"
#include "AstCore/DragForce.hpp"
#include "AstCore/EventDetector.hpp"
#include "AstCore/GravityForce.hpp"
#include "AstCore/HPOPForceModel.hpp"
#include "AstCore/InitialState.hpp"
#include "AstCore/MainSequence.hpp"
#include "AstCore/Maneuver.hpp"
#include "AstCore/Propagate.hpp"
#include "AstCore/Sequence.hpp"
#include "AstCore/SolarRadiationPressure.hpp"
#include "AstCore/SpacecraftState.hpp"
#include "AstCore/StateCartesian.hpp"
#include "AstCore/StateKeplerian.hpp"
#include "AstCore/TargeterSequence.hpp"
#include "AstCore/ThirdBodyForce.hpp"

#include "AstMath/ODEVarStepIntegrator.hpp"

#include "AstSim/MotionTwoBody.hpp"

#include "AstAnalyzer/StudyWorkbench.hpp"
#include "AstAnalyzer/SweepStudy.hpp"

#include "AstGUI/UiAttributeTree.hpp"
#include "AstGUI/UiBodyEphemerisDE.hpp"
#include "AstGUI/UiBodyEphemerisSPK.hpp"
#include "AstGUI/UiBurnImpulsive.hpp"
#include "AstGUI/UiCelestialBody.hpp"
#include "AstGUI/UiDragForce.hpp"
#include "AstGUI/UiEventDetector.hpp"
#include "AstGUI/UiGravityForce.hpp"
#include "AstGUI/UiHPOPForceModel.hpp"
#include "AstGUI/UiInitialState.hpp"
#include "AstGUI/UiSequenceWorkbench.hpp"
#include "AstGUI/UiManeuver.hpp"
#include "AstGUI/UiMotionTwoBody.hpp"
#include "AstGUI/UiODEVarStepIntegrator.hpp"
#include "AstGUI/UiPropagate.hpp"
#include "AstGUI/UiSequence.hpp"
#include "AstGUI/UiSolarRadiationPressure.hpp"
#include "AstGUI/UiSpacecraftParams.hpp"
#include "AstGUI/UiStateCartesian.hpp"
#include "AstGUI/UiStateKeplerian.hpp"
#include "AstGUI/UiStudyWorkbench.hpp"
#include "AstGUI/UiSweepStudy.hpp"
#include "AstGUI/UiThirdBodyForce.hpp"
#include "AstGUI/AstGUIAPI.hpp"

AST_NAMESPACE_BEGIN

ObjectEditRegistry::ObjectEditRegistry()
{

}

template<typename T>
void aUiRegisterEditWidget(ObjectEditRegistry *registry, FNewEditWidget newEditWidget)
{
    static_assert(has_own_getType<T>::value, "registerEditWidget requires the type to has a AST_OBJECT macro");
    registry->registerEditWidget(T::StaticType(), newEditWidget);
}

ObjectEditRegistry::ObjectEditRegistry(bool shouldRegistEditWidget)
{
    if(shouldRegistEditWidget)
    {
        aGUIInit();

        // 基类（属性树兜底）
        aUiRegisterEditWidget<Object>(this, [](Object *object) -> QWidget* {
            auto* editor = new UiAttributeTree();
            editor->setObject(object);
            return editor;
        });

        // 状态
        aUiRegisterEditWidget<StateCartesian>(this, [](Object *object) -> QWidget* { return new UiStateCartesian(object); });
        aUiRegisterEditWidget<StateKeplerian>(this, [](Object *object) -> QWidget* { return new UiStateKeplerian(object); });

        // 运动模型
        aUiRegisterEditWidget<MotionTwoBody>(this, [](Object *object) -> QWidget* { return new UiMotionTwoBody(object); });

        // 天体
        aUiRegisterEditWidget<BodyEphemerisDE>(this, [](Object *object) -> QWidget* { return new UiBodyEphemerisDE(object); });
        aUiRegisterEditWidget<BodyEphemerisSPK>(this, [](Object *object) -> QWidget* { return new UiBodyEphemerisSPK(object); });
        aUiRegisterEditWidget<CelestialBody>(this, [](Object *object) -> QWidget* { return new UiCelestialBody(object); });

        // 力模型
        aUiRegisterEditWidget<GravityForce>(this, [](Object *object) -> QWidget* { return new UiGravityForce(object); });
        aUiRegisterEditWidget<DragForce>(this, [](Object *object) -> QWidget* { return new UiDragForce(object); });
        aUiRegisterEditWidget<SolarRadiationPressure>(this, [](Object *object) -> QWidget* { return new UiSolarRadiationPressure(object); });
        aUiRegisterEditWidget<ThirdBodyForce>(this, [](Object *object) -> QWidget* { return new UiThirdBodyForce(object); });
        aUiRegisterEditWidget<HPOPForceModel>(this, [](Object *object) -> QWidget* { return new UiHPOPForceModel(object); });

        // 任务序列
        aUiRegisterEditWidget<Sequence>(this, [](Object *object) -> QWidget* {
            if(object)
            {
                auto parent = aobject_cast<Sequence*>(object->getParentScope());
                // 父对象不是任务序列，说明是主任务序列，则采用任务序列工作台编辑器
                if(!parent)
                {
                    return new UiSequenceWorkbench(object);
                }
            } 
            return new UiSequence(object); 
        });
        aUiRegisterEditWidget<Propagate>(this, [](Object *object) -> QWidget* { return new UiPropagate(object); });
        aUiRegisterEditWidget<Maneuver>(this, [](Object *object) -> QWidget* { return new UiManeuver(object); });
        aUiRegisterEditWidget<TargeterSequence>(this, [](Object *object) -> QWidget* { return new UiTargeterSequence(object); });
        aUiRegisterEditWidget<InitialState>(this, [](Object *object) -> QWidget* { return new UiInitialState(object); });

        // 航天器与部件
        aUiRegisterEditWidget<SpacecraftState>(this, [](Object *object) -> QWidget* { return new UiSpacecraftParams(object); });
        aUiRegisterEditWidget<BurnImpulsive>(this, [](Object *object) -> QWidget* { return new UiBurnImpulsive(object); });

        // 传播器
        aUiRegisterEditWidget<EventDetector>(this, [](Object *object) -> QWidget* { return new UiEventDetector(object); });

        // 数学
        aUiRegisterEditWidget<ODEVarStepIntegrator>(this, [](Object *object) -> QWidget* { return new UiODEVarStepIntegrator(object); });

        // 分析
        aUiRegisterEditWidget<StudyWorkbench>(this, [](Object *object) -> QWidget* {
            auto* editor = new UiStudyWorkbench();
            editor->setStudyWorkbench(static_cast<StudyWorkbench*>(object));
            return editor;
        });
        aUiRegisterEditWidget<SweepStudy>(this, [](Object *object) -> QWidget* {
            auto* editor = new UiSweepStudy();
            editor->setAnalyzer(static_cast<SweepStudy*>(object));
            return editor;
        });
    }
}

ObjectEditRegistry &ObjectEditRegistry::Instance()
{
    static ObjectEditRegistry instance(true);
    return instance;
}

void ObjectEditRegistry::registerEditWidget(Class *cls, FNewEditWidget newEditWidget)
{
    editWidgetMap_[cls] = newEditWidget;
}

QWidget *ObjectEditRegistry::newEditWidget(Object *object)
{
    if(!object)
        return nullptr;
    Class *cls = object->getType();
    while(cls)
    {
        auto it = editWidgetMap_.find(cls);
        if(it == editWidgetMap_.end())
        {
            cls = cls->getParent();
        }
        else
        {
            return it->second(object);
        }
    }
    return nullptr;
}

void aUiRegisterEditWidget(Class *cls, FNewEditWidget newEditWidget)
{
    ObjectEditRegistry::Instance().registerEditWidget(cls, newEditWidget);
}

QWidget *aUiNewEditWidget(Object *object)
{
    return ObjectEditRegistry::Instance().newEditWidget(object);
}

AST_NAMESPACE_END