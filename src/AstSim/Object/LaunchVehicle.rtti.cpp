#include "LaunchVehicle.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class LaunchVehicle::staticType;

static bool LaunchVehicle_ClassInited = (LaunchVehicle::ClassInit(&LaunchVehicle::staticType), true);

void LaunchVehicle::ClassInit(Class* cls)
{

    cls->setName("LaunchVehicle");
    cls->setDesc(u8R"(运载火箭对象)");
    cls->addToRegistry();
    cls->setParent<Spacecraft>();
    cls->setConstructor<LaunchVehicle>();

}

AST_NAMESPACE_END