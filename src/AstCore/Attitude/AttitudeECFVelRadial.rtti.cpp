#include "AttitudeECFVelRadial.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class AttitudeECFVelRadial::staticType;

static bool AttitudeECFVelRadial_ClassInited = (AttitudeECFVelRadial::ClassInit(&AttitudeECFVelRadial::staticType), true);

void AttitudeECFVelRadial::ClassInit(Class* cls)
{
    cls->setName("AttitudeECFVelRadial");
    cls->setDesc(u8R"(ECF 速度对齐 + 径向约束姿态)");
    cls->addToRegistry();
    cls->setParent<AttitudeTrajectoryRelated>();
    cls->setConstructor<AttitudeECFVelRadial>();
}

AST_NAMESPACE_END
