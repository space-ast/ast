#include "AttitudeSpinning.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class AttitudeSpinning::staticType;

static bool AttitudeSpinning_ClassInited = (AttitudeSpinning::ClassInit(&AttitudeSpinning::staticType), true);

void AttitudeSpinning::ClassInit(Class* cls)
{
    cls->setName("AttitudeSpinning");
    cls->setDesc(u8R"(自旋姿态剖面（STK: Spinning）)");
    cls->addToRegistry();
    cls->setParent<AttitudeProfile>();
    cls->setConstructor<AttitudeSpinning>();
    cls->addProperty("SpinRate", aNewPropertyDouble<AttitudeSpinning, &AttitudeSpinning::getSpinRate, &AttitudeSpinning::setSpinRate>());
    cls->addProperty("SpinOffset", aNewPropertyDouble<AttitudeSpinning, &AttitudeSpinning::getSpinOffset, &AttitudeSpinning::setSpinOffset>());
}

AST_NAMESPACE_END
