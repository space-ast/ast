#include "LaunchVehicle.hpp"

#include "AstCore/ReflectAPI.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class LaunchVehicle::staticType;

Class* LaunchVehicle::StaticType()
{
    return &staticType;
}

Class* LaunchVehicle::getType() const
{
    return &staticType;
}

static bool LaunchVehicle_ClassInited = (LaunchVehicle::ClassInit(&LaunchVehicle::staticType), true);

void LaunchVehicle::ClassInit(Class* cls)
{

    cls->setName(NC_("Class", "LaunchVehicle"));
    cls->setDesc(u8R"(运载火箭对象)");
    cls->addToRegistry();
    cls->setParent<Spacecraft>();
    cls->setConstructor<LaunchVehicle>();

}

AST_NAMESPACE_END