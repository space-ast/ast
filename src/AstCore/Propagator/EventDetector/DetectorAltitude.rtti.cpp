#include "DetectorAltitude.hpp"

#include "AstCore/ReflectAPI.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class DetectorAltitude::staticType;

Class* DetectorAltitude::StaticType()
{
    return &staticType;
}

Class* DetectorAltitude::getType() const
{
    return &staticType;
}

static bool DetectorAltitude_ClassInited = (DetectorAltitude::ClassInit(&DetectorAltitude::staticType), true);

void DetectorAltitude::ClassInit(Class* cls)
{

    cls->setName(NC_("Class", "DetectorAltitude"));
    cls->addToRegistry();
    cls->setParent<DetectorBodyRelated>();
    cls->setConstructor<DetectorAltitude>();

}

AST_NAMESPACE_END