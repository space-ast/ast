#include "SolarRadiationPressure.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class SolarRadiationPressure::staticType;

static bool SolarRadiationPressure_ClassInited = (SolarRadiationPressure::ClassInit(&SolarRadiationPressure::staticType), true);

void SolarRadiationPressure::ClassInit(Class* cls)
{

    cls->setName(N_("SolarRadiationPressure"));
    cls->setDesc(u8R"(太阳辐射压力)");
    cls->addToRegistry();
    cls->setParent<ForceModel>();
    cls->setConstructor<SolarRadiationPressure>();

}

AST_NAMESPACE_END