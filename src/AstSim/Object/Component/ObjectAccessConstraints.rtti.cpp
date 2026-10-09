#include "ObjectAccessConstraints.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class ObjectAccessConstraints::staticType;

static bool ObjectAccessConstraints_ClassInited = (ObjectAccessConstraints::ClassInit(&ObjectAccessConstraints::staticType), true);

void ObjectAccessConstraints::ClassInit(Class* cls)
{

    cls->setName(NC_("Class", "ObjectAccessConstraints"));
    cls->addToRegistry();
    cls->setParent<ObjectNamed>();
    cls->setConstructor<ObjectAccessConstraints>();

}

AST_NAMESPACE_END