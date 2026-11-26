#include "DetectorXYPlaneCross.hpp"

#include "AstCore/ReflectAPI.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class DetectorXYPlaneCross::staticType;

Class* DetectorXYPlaneCross::StaticType()
{
    return &staticType;
}

Class* DetectorXYPlaneCross::getType() const
{
    return &staticType;
}

static bool DetectorXYPlaneCross_ClassInited = (DetectorXYPlaneCross::ClassInit(&DetectorXYPlaneCross::staticType), true);

void DetectorXYPlaneCross::ClassInit(Class* cls)
{

    cls->setName(NC_("Class", "DetectorXYPlaneCross"));
    cls->addToRegistry();
    cls->setParent<EventDetector>();
    cls->setConstructor<DetectorXYPlaneCross>();

}

AST_NAMESPACE_END