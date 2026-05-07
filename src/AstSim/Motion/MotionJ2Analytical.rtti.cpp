#include "MotionJ2Analytical.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class MotionJ2Analytical::staticType;

static bool MotionJ2Analytical_ClassInited = (MotionJ2Analytical::ClassInit(&MotionJ2Analytical::staticType), true);

void MotionJ2Analytical::ClassInit(Class* cls)
{

    cls->setName("MotionJ2Analytical");
    cls->setDesc(u8R"(J2解析轨道动力学)");
    cls->addToRegistry();
    cls->setParent<MotionOrbitDynamics>();
    cls->setConstructor<MotionJ2Analytical>();

}

AST_NAMESPACE_END