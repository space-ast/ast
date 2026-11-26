#include "OrConstraint.hpp"

#include "AstCore/ReflectAPI.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class OrConstraint::staticType;

Class* OrConstraint::StaticType()
{
    return &staticType;
}

Class* OrConstraint::getType() const
{
    return &staticType;
}

static bool OrConstraint_ClassInited = (OrConstraint::ClassInit(&OrConstraint::staticType), true);

void OrConstraint::ClassInit(Class* cls)
{

    cls->setName(NC_("Class", "OrConstraint"));
    cls->setDesc(u8R"(或约束组合器)");
    cls->addToRegistry();
    cls->setParent<AccessConstraint>();
    cls->setConstructor<OrConstraint>();

}

AST_NAMESPACE_END