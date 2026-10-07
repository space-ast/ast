#include "AttitudeYPRFixedECI.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class AttitudeYPRFixedECI::staticType;

static bool AttitudeYPRFixedECI_ClassInited = (AttitudeYPRFixedECI::ClassInit(&AttitudeYPRFixedECI::staticType), true);

void AttitudeYPRFixedECI::ClassInit(Class* cls)
{
    cls->setName("AttitudeYPRFixedECI");
    cls->setDesc(u8R"(偏航-俯仰-滚转固定姿态剖面)");
    cls->addToRegistry();
    cls->setParent<AttitudeFixed>();
    cls->setConstructor<AttitudeYPRFixedECI>();
}

AST_NAMESPACE_END
