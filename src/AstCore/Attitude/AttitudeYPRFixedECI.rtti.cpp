#include "AttitudeYPRFixedECI.hpp"

#include "AstCore/ReflectAPI.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class AttitudeYPRFixedECI::staticType;

Class* AttitudeYPRFixedECI::StaticType()
{
    return &staticType;
}

Class* AttitudeYPRFixedECI::getType() const
{
    return &staticType;
}

static bool AttitudeYPRFixedECI_ClassInited = (AttitudeYPRFixedECI::ClassInit(&AttitudeYPRFixedECI::staticType), true);

void AttitudeYPRFixedECI::ClassInit(Class* cls)
{

    cls->setName(NC_("Class", "AttitudeYPRFixedECI"));
    cls->setDesc(u8R"(偏航-俯仰-滚转固定姿态剖面 (YPRFixedECI))");
    cls->addToRegistry();
    cls->setParent<AttitudeFixed>();
    cls->setConstructor<AttitudeYPRFixedECI>();

}

AST_NAMESPACE_END