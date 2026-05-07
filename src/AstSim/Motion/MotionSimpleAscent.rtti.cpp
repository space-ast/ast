#include "MotionSimpleAscent.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class MotionSimpleAscent::staticType;

static bool MotionSimpleAscent_ClassInited = (MotionSimpleAscent::ClassInit(&MotionSimpleAscent::staticType), true);

void MotionSimpleAscent::ClassInit(Class* cls)
{

    cls->setName("MotionSimpleAscent");
    cls->setDesc(u8R"(简单上升运动模型，用于模拟运载火箭的上升阶段运动)");
    cls->addToRegistry();
    cls->setParent<MotionProfile>();
    cls->setConstructor<MotionSimpleAscent>();

}

AST_NAMESPACE_END