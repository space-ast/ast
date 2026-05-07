#include "MotionOrbitDynamics.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class MotionOrbitDynamics::staticType;

static bool MotionOrbitDynamics_ClassInited = (MotionOrbitDynamics::ClassInit(&MotionOrbitDynamics::staticType), true);

void MotionOrbitDynamics::ClassInit(Class* cls)
{

    cls->setName("MotionOrbitDynamics");
    cls->setDesc(u8R"(轨道动力学基础运动模型数据结构)");
    cls->addToRegistry();
    cls->setParent<MotionWithIntervalStep>();
    cls->setConstructor<MotionOrbitDynamics>();

    cls->addProperty("InitialState", aNewPropertyObject<MotionOrbitDynamics, State, &MotionOrbitDynamics::getInitialState, &MotionOrbitDynamics::setInitialState>());
    cls->addProperty("PropagationFrame", aNewPropertyObject<MotionOrbitDynamics, Frame, &MotionOrbitDynamics::getPropagationFrame, &MotionOrbitDynamics::setPropagationFrame>());
}

AST_NAMESPACE_END