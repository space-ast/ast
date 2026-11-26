#include "Burn.hpp"

#include "AstCore/ReflectAPI.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class Burn::staticType;

Class* Burn::StaticType()
{
    return &staticType;
}

Class* Burn::getType() const
{
    return &staticType;
}

static bool Burn_ClassInited = (Burn::ClassInit(&Burn::staticType), true);

void Burn::ClassInit(Class* cls)
{

    cls->setName(NC_("Class", "Burn"));
    cls->setDesc(u8R"(发动机点火)");
    cls->addToRegistry();
    cls->setParent<ObjectNamed>();
    cls->setConstructor<Burn>();

}

AST_NAMESPACE_END