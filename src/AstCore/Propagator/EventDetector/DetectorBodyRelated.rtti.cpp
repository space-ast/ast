#include "DetectorBodyRelated.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class DetectorBodyRelated::staticType;

static bool DetectorBodyRelated_ClassInited = (DetectorBodyRelated::ClassInit(&DetectorBodyRelated::staticType), true);

void DetectorBodyRelated::ClassInit(Class* cls)
{

    cls->setName("DetectorBodyRelated");
    cls->addToRegistry();
    cls->setParent<EventDetector>();
    cls->setConstructor<DetectorBodyRelated>();

    cls->addProperty("body", aNewPropertyObject<DetectorBodyRelated, Body, &DetectorBodyRelated::body, &DetectorBodyRelated::setBody>());
}

AST_NAMESPACE_END