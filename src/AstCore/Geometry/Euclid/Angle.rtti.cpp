#include "Angle.hpp"

#include "AstCore/ReflectAPI.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class Angle::staticType;

Class* Angle::StaticType()
{
    return &staticType;
}

Class* Angle::getType() const
{
    return &staticType;
}

static bool Angle_ClassInited = (Angle::ClassInit(&Angle::staticType), true);

void Angle::ClassInit(Class* cls)
{

    cls->setName(NC_("Class", "Angle"));
    cls->setDesc(u8R"(角度抽象基类)");
    cls->addToRegistry();
    cls->setParent<ObjectNamed>();
    cls->setConstructor<Angle>();

}

AST_NAMESPACE_END