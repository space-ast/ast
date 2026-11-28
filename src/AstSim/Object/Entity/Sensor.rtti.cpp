#include "Sensor.hpp"

#include "AstCore/ReflectAPI.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class Sensor::staticType;

Class* Sensor::StaticType()
{
    return &staticType;
}

Class* Sensor::getType() const
{
    return &staticType;
}

static bool Sensor_ClassInited = (Sensor::ClassInit(&Sensor::staticType), true);

void Sensor::ClassInit(Class* cls)
{

    cls->setName(NC_("Class", "Sensor"));
    cls->setDesc(u8R"(传感器对象)");
    cls->addToRegistry();
    cls->setParent<Platform>();
    cls->setConstructor<Sensor>();

}

AST_NAMESPACE_END