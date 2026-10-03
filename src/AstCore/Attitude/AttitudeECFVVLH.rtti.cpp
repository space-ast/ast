#include "AttitudeECFVVLH.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class AttitudeECFVVLH::staticType;

static bool AttitudeECFVVLH_ClassInited = (AttitudeECFVVLH::ClassInit(&AttitudeECFVVLH::staticType), true);

void AttitudeECFVVLH::ClassInit(Class* cls)
{
    cls->setName("AttitudeECFVVLH");
    cls->setDesc(u8R"(对地指向 + ECF 速度约束姿态)");
    cls->addToRegistry();
    cls->setParent<AttitudeVVLH>();
    cls->setConstructor<AttitudeECFVVLH>();
}

AST_NAMESPACE_END
