#include "DetectorXYPlaneCross.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class DetectorXYPlaneCross::staticType;

static bool DetectorXYPlaneCross_ClassInited = (DetectorXYPlaneCross::ClassInit(&DetectorXYPlaneCross::staticType), true);

void DetectorXYPlaneCross::ClassInit(Class* cls)
{

    cls->setName(N_("DetectorXYPlaneCross"));
    cls->addToRegistry();
    cls->setParent<EventDetector>();
    cls->setConstructor<DetectorXYPlaneCross>();

}

AST_NAMESPACE_END