#include "DistanceConstraint.hpp"

#include "AstCore/ReflectAPI.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class DistanceConstraint::staticType;

Class* DistanceConstraint::StaticType()
{
    return &staticType;
}

Class* DistanceConstraint::getType() const
{
    return &staticType;
}

static bool DistanceConstraint_ClassInited = (DistanceConstraint::ClassInit(&DistanceConstraint::staticType), true);

void DistanceConstraint::ClassInit(Class* cls)
{

    cls->setName(NC_("Class", "DistanceConstraint"));
    cls->setDesc(u8R"(两点距离约束)");
    cls->addToRegistry();
    cls->setParent<AccessConstraint>();
    cls->setConstructor<DistanceConstraint>();

}

AST_NAMESPACE_END