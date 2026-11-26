#include "AttitudeVVLH.hpp"

#include "AstCore/ReflectAPI.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class AttitudeVVLH::staticType;

Class* AttitudeVVLH::StaticType()
{
    return &staticType;
}

Class* AttitudeVVLH::getType() const
{
    return &staticType;
}

static bool AttitudeVVLH_ClassInited = (AttitudeVVLH::ClassInit(&AttitudeVVLH::staticType), true);

void AttitudeVVLH::ClassInit(Class* cls)
{

    cls->setName(NC_("Class", "AttitudeVVLH"));
    cls->setDesc(u8R"(速度局部水平姿态(对原点定向 + 速度约束))");
    cls->addToRegistry();
    cls->setParent<AttitudeTrajectoryRelated>();
    cls->setConstructor<AttitudeVVLH>();

}

AST_NAMESPACE_END