#include "FixedStepAccessStepper.hpp"

#include "AstCore/ReflectAPI.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class FixedStepAccessStepper::staticType;

Class* FixedStepAccessStepper::StaticType()
{
    return &staticType;
}

Class* FixedStepAccessStepper::getType() const
{
    return &staticType;
}

static bool FixedStepAccessStepper_ClassInited = (FixedStepAccessStepper::ClassInit(&FixedStepAccessStepper::staticType), true);

void FixedStepAccessStepper::ClassInit(Class* cls)
{

    cls->setName(NC_("Class", "FixedStepAccessStepper"));
    cls->setDesc(u8R"(固定步长步进器)");
    cls->addToRegistry();
    cls->setParent<AccessStepper>();
    cls->setConstructor<FixedStepAccessStepper>();

}

AST_NAMESPACE_END