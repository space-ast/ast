#include "GroundVehicle.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class GroundVehicle::staticType;

static bool GroundVehicle_ClassInited = (GroundVehicle::ClassInit(&GroundVehicle::staticType), true);

void GroundVehicle::ClassInit(Class* cls)
{

    cls->setName("GroundVehicle");
    cls->setDesc(u8R"(地面车辆对象)");
    cls->addToRegistry();
    cls->setParent<Mover>();
    cls->setConstructor<GroundVehicle>();

}

AST_NAMESPACE_END