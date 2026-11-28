#include "AccessConstraint.hpp"

#include "AstCore/ReflectAPI.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class AccessConstraint::staticType;

Class* AccessConstraint::StaticType()
{
    return &staticType;
}

Class* AccessConstraint::getType() const
{
    return &staticType;
}

static bool AccessConstraint_ClassInited = (AccessConstraint::ClassInit(&AccessConstraint::staticType), true);

void AccessConstraint::ClassInit(Class* cls)
{

    cls->setName(NC_("Class", "AccessConstraint"));
    cls->setDesc(u8R"(访问约束抽象基类)");
    cls->addToRegistry();
    cls->setParent<ObjectNamed>();
    cls->setConstructor<AccessConstraint>();

}

AST_NAMESPACE_END