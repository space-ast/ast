#include "AttitudeAlignConstrain.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class AttitudeAlignConstrain::staticType;

static bool AttitudeAlignConstrain_ClassInited = (AttitudeAlignConstrain::ClassInit(&AttitudeAlignConstrain::staticType), true);

void AttitudeAlignConstrain::ClassInit(Class* cls)
{
    cls->setName("AttitudeAlignConstrain");
    cls->setDesc(u8R"(对齐/约束姿态剖面)");
    cls->addToRegistry();
    cls->setParent<AttitudeProfile>();
    cls->setConstructor<AttitudeAlignConstrain>();

    cls->addProperty("Azimuth", aNewPropertyDouble<AttitudeAlignConstrain, &AttitudeAlignConstrain::getAzimuth, &AttitudeAlignConstrain::setAzimuth>());
}

AST_NAMESPACE_END
