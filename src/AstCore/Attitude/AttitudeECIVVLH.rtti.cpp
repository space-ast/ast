#include "AttitudeECIVVLH.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class AttitudeECIVVLH::staticType;

static bool AttitudeECIVVLH_ClassInited = (AttitudeECIVVLH::ClassInit(&AttitudeECIVVLH::staticType), true);

void AttitudeECIVVLH::ClassInit(Class* cls)
{
    cls->setName("AttitudeECIVVLH");
    cls->setDesc(u8R"(对地指向 + ECI 速度约束姿态)");
    cls->addToRegistry();
    cls->setParent<AttitudeVVLH>();
    cls->setConstructor<AttitudeECIVVLH>();
}

AST_NAMESPACE_END
