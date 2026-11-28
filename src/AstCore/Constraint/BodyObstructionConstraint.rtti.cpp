#include "BodyObstructionConstraint.hpp"

#include "AstCore/ReflectAPI.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class BodyObstructionConstraint::staticType;

Class* BodyObstructionConstraint::StaticType()
{
    return &staticType;
}

Class* BodyObstructionConstraint::getType() const
{
    return &staticType;
}

static bool BodyObstructionConstraint_ClassInited = (BodyObstructionConstraint::ClassInit(&BodyObstructionConstraint::staticType), true);

void BodyObstructionConstraint::ClassInit(Class* cls)
{

    cls->setName(NC_("Class", "BodyObstructionConstraint"));
    cls->setDesc(u8R"(天体遮挡约束)");
    cls->addToRegistry();
    cls->setParent<AccessConstraint>();
    cls->setConstructor<BodyObstructionConstraint>();

}

AST_NAMESPACE_END