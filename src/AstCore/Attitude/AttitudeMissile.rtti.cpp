#include "AttitudeMissile.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class AttitudeMissile::staticType;

static bool AttitudeMissile_ClassInited = (AttitudeMissile::ClassInit(&AttitudeMissile::staticType), true);

void AttitudeMissile::ClassInit(Class* cls)
{

    cls->setName(NC_("Class", "AttitudeMissile"));
    cls->setDesc(u8R"(ECI 速度对齐 + 对地约束姿态)");
    cls->addToRegistry();
    cls->setParent<AttitudeTrajectoryRelated>();
    cls->setConstructor<AttitudeMissile>();

}

AST_NAMESPACE_END