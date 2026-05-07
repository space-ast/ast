#include "DetectorDuration.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class DetectorDuration::staticType;

static bool DetectorDuration_ClassInited = (DetectorDuration::ClassInit(&DetectorDuration::staticType), true);

void DetectorDuration::ClassInit(Class* cls)
{

    cls->setName("DetectorDuration");
    cls->addToRegistry();
    cls->setParent<EventDetector>();
    cls->setConstructor<DetectorDuration>();

    cls->addProperty("duration", aNewPropertyDouble<DetectorDuration, &DetectorDuration::duration, &DetectorDuration::setDuration>());
}

AST_NAMESPACE_END