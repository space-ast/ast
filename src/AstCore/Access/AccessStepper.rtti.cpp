#include "AccessStepper.hpp"

#include "AstCore/ReflectAPI.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class AccessStepper::staticType;

Class* AccessStepper::StaticType()
{
    return &staticType;
}

Class* AccessStepper::getType() const
{
    return &staticType;
}

static bool AccessStepper_ClassInited = (AccessStepper::ClassInit(&AccessStepper::staticType), true);

void AccessStepper::ClassInit(Class* cls)
{

    cls->setName(NC_("Class", "AccessStepper"));
    cls->setDesc(u8R"(步进策略抽象基类)");
    cls->addToRegistry();
    cls->setParent<ObjectNamed>();
    cls->setConstructor<AccessStepper>();

}

AST_NAMESPACE_END