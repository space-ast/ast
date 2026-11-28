#include "AndConstraint.hpp"

#include "AstCore/ReflectAPI.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class AndConstraint::staticType;

Class* AndConstraint::StaticType()
{
    return &staticType;
}

Class* AndConstraint::getType() const
{
    return &staticType;
}

static bool AndConstraint_ClassInited = (AndConstraint::ClassInit(&AndConstraint::staticType), true);

void AndConstraint::ClassInit(Class* cls)
{

    cls->setName(NC_("Class", "AndConstraint"));
    cls->setDesc(u8R"(与约束组合器)");
    cls->addToRegistry();
    cls->setParent<AccessConstraint>();
    cls->setConstructor<AndConstraint>();

}

AST_NAMESPACE_END