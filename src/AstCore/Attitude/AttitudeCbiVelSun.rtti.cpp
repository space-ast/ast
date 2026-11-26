#include "AttitudeCbiVelSun.hpp"

#include "AstCore/ReflectAPI.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class AttitudeCbiVelSun::staticType;

Class* AttitudeCbiVelSun::StaticType()
{
    return &staticType;
}

Class* AttitudeCbiVelSun::getType() const
{
    return &staticType;
}

static bool AttitudeCbiVelSun_ClassInited = (AttitudeCbiVelSun::ClassInit(&AttitudeCbiVelSun::staticType), true);

void AttitudeCbiVelSun::ClassInit(Class* cls)
{

    cls->setName(NC_("Class", "AttitudeCbiVelSun"));
    cls->setDesc(u8R"(CBI 速度对齐 + 太阳方向约束姿态)");
    cls->addToRegistry();
    cls->setParent<AttitudeSunRelated>();
    cls->setConstructor<AttitudeCbiVelSun>();

}

AST_NAMESPACE_END