#include "Constellation.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class Constellation::staticType;

static bool Constellation_ClassInited = (Constellation::ClassInit(&Constellation::staticType), true);

void Constellation::ClassInit(Class* cls)
{

    cls->setName("Constellation");
    cls->setDesc(u8R"(星座对象)");
    cls->addToRegistry();
    cls->setParent<ObjectNamed>();
    cls->setConstructor<Constellation>();

}

AST_NAMESPACE_END