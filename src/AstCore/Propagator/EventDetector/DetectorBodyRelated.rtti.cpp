#include "DetectorBodyRelated.hpp"

#include "AstCore/ReflectAPI.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class DetectorBodyRelated::staticType;

Class* DetectorBodyRelated::StaticType()
{
    return &staticType;
}

Class* DetectorBodyRelated::getType() const
{
    return &staticType;
}

static bool DetectorBodyRelated_ClassInited = (DetectorBodyRelated::ClassInit(&DetectorBodyRelated::staticType), true);

void DetectorBodyRelated::ClassInit(Class* cls)
{

    cls->setName(NC_("Class", "DetectorBodyRelated"));
    cls->addToRegistry();
    cls->setParent<EventDetector>();
    cls->setConstructor<DetectorBodyRelated>();

    cls->addProperty("body", aNewPropertyObject<DetectorBodyRelated, Body, &DetectorBodyRelated::body, &DetectorBodyRelated::setBody>());
}

AST_NAMESPACE_END