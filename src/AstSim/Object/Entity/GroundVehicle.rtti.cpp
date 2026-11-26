#include "GroundVehicle.hpp"

#include "AstCore/ReflectAPI.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class GroundVehicle::staticType;

Class* GroundVehicle::StaticType()
{
    return &staticType;
}

Class* GroundVehicle::getType() const
{
    return &staticType;
}

static bool GroundVehicle_ClassInited = (GroundVehicle::ClassInit(&GroundVehicle::staticType), true);

void GroundVehicle::ClassInit(Class* cls)
{

    cls->setName(NC_("Class", "GroundVehicle"));
    cls->setDesc(u8R"(地面车辆对象)");
    cls->addToRegistry();
    cls->setParent<Mover>();
    cls->setConstructor<GroundVehicle>();

}

AST_NAMESPACE_END