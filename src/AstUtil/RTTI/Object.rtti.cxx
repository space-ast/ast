#include "Object.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class Object::staticType;

static bool Object_ClassInited = (Object::ClassInit(&Object::staticType), true);

void Object::ClassInit(Class* cls)
{

    cls->setName("Object");
    cls->addToRegistry();
    cls->setParent(nullptr);
    cls->setConstructor<Object>();
    cls->setResolve<Object>();

}

AST_NAMESPACE_END