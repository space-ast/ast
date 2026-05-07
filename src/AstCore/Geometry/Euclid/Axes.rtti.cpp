#include "Axes.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class Axes::staticType;

static bool Axes_ClassInited = (Axes::ClassInit(&Axes::staticType), true);

void Axes::ClassInit(Class* cls)
{

    cls->setName("Axes");
    cls->setDesc(u8R"(轴系类)");
    cls->addToRegistry();
    cls->setParent<ObjectNamed>();
    cls->setConstructor<Axes>();

}

AST_NAMESPACE_END