#include "FieldOfViewConstraint.hpp"

#include "AstCore/ReflectAPI.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class FieldOfViewConstraint::staticType;

Class* FieldOfViewConstraint::StaticType()
{
    return &staticType;
}

Class* FieldOfViewConstraint::getType() const
{
    return &staticType;
}

static bool FieldOfViewConstraint_ClassInited = (FieldOfViewConstraint::ClassInit(&FieldOfViewConstraint::staticType), true);

void FieldOfViewConstraint::ClassInit(Class* cls)
{

    cls->setName(NC_("Class", "FieldOfViewConstraint"));
    cls->setDesc(u8R"(视场约束)");
    cls->addToRegistry();
    cls->setParent<AccessConstraint>();
    cls->setConstructor<FieldOfViewConstraint>();

}

AST_NAMESPACE_END