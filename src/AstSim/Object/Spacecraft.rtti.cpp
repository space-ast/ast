#include "Spacecraft.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class Spacecraft::staticType;

static bool Spacecraft_ClassInited = (Spacecraft::ClassInit(&Spacecraft::staticType), true);

void Spacecraft::ClassInit(Class* cls)
{

    cls->setName("Spacecraft");
    cls->setDesc(u8R"(航天器对象)");
    cls->addToRegistry();
    cls->setParent<SpaceObject>();
    cls->setConstructor<Spacecraft>();

}

AST_NAMESPACE_END