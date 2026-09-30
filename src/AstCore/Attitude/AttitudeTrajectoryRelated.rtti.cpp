#include "AttitudeTrajectoryRelated.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class AttitudeTrajectoryRelated::staticType;

static bool AttitudeTrajectoryRelated_ClassInited = (AttitudeTrajectoryRelated::ClassInit(&AttitudeTrajectoryRelated::staticType), true);

void AttitudeTrajectoryRelated::ClassInit(Class* cls)
{

    cls->setName(NC_("Class", "AttitudeTrajectoryRelated"));
    cls->setDesc(u8R"(与轨迹相关的姿态)");
    cls->addToRegistry();
    cls->setParent<AttitudeProfile>();
    cls->setConstructor<AttitudeTrajectoryRelated>();

}

AST_NAMESPACE_END