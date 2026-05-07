#include "Volumetric.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class Volumetric::staticType;

static bool Volumetric_ClassInited = (Volumetric::ClassInit(&Volumetric::staticType), true);

void Volumetric::ClassInit(Class* cls)
{

    cls->setName("Volumetric");
    cls->setDesc(u8R"(体积对象)");
    cls->addToRegistry();
    cls->setParent<ObjectNamed>();
    cls->setConstructor<Volumetric>();

}

AST_NAMESPACE_END