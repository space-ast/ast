#include "MotionTwoBody.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class MotionTwoBody::staticType;

static bool MotionTwoBody_ClassInited = (MotionTwoBody::ClassInit(&MotionTwoBody::staticType), true);

void MotionTwoBody::ClassInit(Class* cls)
{

    cls->setName("MotionTwoBody");
    cls->setDesc(u8R"(二体问题轨道动力学模型)");
    cls->addToRegistry();
    cls->setParent<MotionOrbitDynamics>();
    cls->setConstructor<MotionTwoBody>();

}

AST_NAMESPACE_END