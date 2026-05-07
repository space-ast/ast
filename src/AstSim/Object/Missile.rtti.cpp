#include "Missile.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class Missile::staticType;

static bool Missile_ClassInited = (Missile::ClassInit(&Missile::staticType), true);

void Missile::ClassInit(Class* cls)
{

    cls->setName("Missile");
    cls->setDesc(u8R"(导弹对象)");
    cls->addToRegistry();
    cls->setParent<Mover>();
    cls->setConstructor<Missile>();

}

AST_NAMESPACE_END