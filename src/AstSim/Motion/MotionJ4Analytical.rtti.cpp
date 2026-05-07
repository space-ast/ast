#include "MotionJ4Analytical.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class MotionJ4Analytical::staticType;

static bool MotionJ4Analytical_ClassInited = (MotionJ4Analytical::ClassInit(&MotionJ4Analytical::staticType), true);

void MotionJ4Analytical::ClassInit(Class* cls)
{

    cls->setName("MotionJ4Analytical");
    cls->setDesc(u8R"(J4解析轨道动力学模型)");
    cls->addToRegistry();
    cls->setParent<MotionOrbitDynamics>();
    cls->setConstructor<MotionJ4Analytical>();

}

AST_NAMESPACE_END