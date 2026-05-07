#include "Submarine.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class Submarine::staticType;

static bool Submarine_ClassInited = (Submarine::ClassInit(&Submarine::staticType), true);

void Submarine::ClassInit(Class* cls)
{

    cls->setName("Submarine");
    cls->setDesc(u8R"(潜艇对象)");
    cls->addToRegistry();
    cls->setParent<Mover>();
    cls->setConstructor<Submarine>();

}

AST_NAMESPACE_END