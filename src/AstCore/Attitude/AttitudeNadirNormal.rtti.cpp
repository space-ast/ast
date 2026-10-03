#include "AttitudeNadirNormal.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class AttitudeNadirNormal::staticType;

static bool AttitudeNadirNormal_ClassInited = (AttitudeNadirNormal::ClassInit(&AttitudeNadirNormal::staticType), true);

void AttitudeNadirNormal::ClassInit(Class* cls)
{
    cls->setName("AttitudeNadirNormal");
    cls->setDesc(u8R"(对地指向 + 轨道法向约束姿态（STK: NadirNormal）)");
    cls->addToRegistry();
    cls->setParent<AttitudeTrajectoryRelated>();
    cls->setConstructor<AttitudeNadirNormal>();
}

AST_NAMESPACE_END
