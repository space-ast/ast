#include "DetectorZXPlaneCross.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class DetectorZXPlaneCross::staticType;

static bool DetectorZXPlaneCross_ClassInited = (DetectorZXPlaneCross::ClassInit(&DetectorZXPlaneCross::staticType), true);

void DetectorZXPlaneCross::ClassInit(Class* cls)
{

    cls->setName(N_("DetectorZXPlaneCross"));
    cls->addToRegistry();
    cls->setParent<EventDetector>();
    cls->setConstructor<DetectorZXPlaneCross>();

}

AST_NAMESPACE_END