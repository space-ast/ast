#include "NotConstraint.hpp"

#include "AstCore/ReflectAPI.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class NotConstraint::staticType;

Class* NotConstraint::StaticType()
{
    return &staticType;
}

Class* NotConstraint::getType() const
{
    return &staticType;
}

static bool NotConstraint_ClassInited = (NotConstraint::ClassInit(&NotConstraint::staticType), true);

void NotConstraint::ClassInit(Class* cls)
{

    cls->setName(NC_("Class", "NotConstraint"));
    cls->setDesc(u8R"(非约束组合器)");
    cls->addToRegistry();
    cls->setParent<AccessConstraint>();
    cls->setConstructor<NotConstraint>();

}

AST_NAMESPACE_END