#include "SpaceObject.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class SpaceObject::staticType;

static bool SpaceObject_ClassInited = (SpaceObject::ClassInit(&SpaceObject::staticType), true);

void SpaceObject::ClassInit(Class* cls)
{

    cls->setName("SpaceObject");
    cls->addToRegistry();
    cls->setParent<Mover>();
    cls->setConstructor<SpaceObject>();

}

AST_NAMESPACE_END