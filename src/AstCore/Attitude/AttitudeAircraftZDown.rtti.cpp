#include "AttitudeAircraftZDown.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class AttitudeAircraftZDown::staticType;

static bool AttitudeAircraftZDown_ClassInited = (AttitudeAircraftZDown::ClassInit(&AttitudeAircraftZDown::staticType), true);

void AttitudeAircraftZDown::ClassInit(Class* cls)
{
    cls->setName("AttitudeAircraftZDown");
    cls->setDesc(u8R"(对地指向 + ECF 速度约束的机体 Z 朝下姿态（STK: AircraftZDown / AircraftZDownAtt）)");
    cls->addToRegistry();
    cls->setParent<AttitudeAlignConstrain>();
    cls->setConstructor<AttitudeAircraftZDown>();
}

AST_NAMESPACE_END
