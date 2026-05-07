#include "State.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class State::staticType;

static bool State_ClassInited = (State::ClassInit(&State::staticType), true);

void State::ClassInit(Class* cls)
{

    cls->setName("State");
    cls->setDesc(u8R"(航天器状态)");
    cls->addToRegistry();
    cls->setParent<ObjectNamed>();
    cls->setConstructor<State>();

    cls->addProperty("Frame", aNewPropertyObject<State, Frame, &State::getFrame, &State::setFrame>());
    cls->addProperty("StateEpoch", aNewPropertyObject<State, EventTime, &State::getStateEpoch, &State::setStateEpoch>());
}

AST_NAMESPACE_END