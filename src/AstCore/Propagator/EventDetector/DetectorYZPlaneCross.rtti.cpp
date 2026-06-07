#include "DetectorYZPlaneCross.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class DetectorYZPlaneCross::staticType;

static bool DetectorYZPlaneCross_ClassInited = (DetectorYZPlaneCross::ClassInit(&DetectorYZPlaneCross::staticType), true);

void DetectorYZPlaneCross::ClassInit(Class* cls)
{

    cls->setName(N_("DetectorYZPlaneCross"));
    cls->addToRegistry();
    cls->setParent<EventDetector>();
    cls->setConstructor<DetectorYZPlaneCross>();

}

AST_NAMESPACE_END