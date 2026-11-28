#include "AttitudeSpinning.hpp"

#include "AstCore/ReflectAPI.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class AttitudeSpinning::staticType;

Class* AttitudeSpinning::StaticType()
{
    return &staticType;
}

Class* AttitudeSpinning::getType() const
{
    return &staticType;
}

static bool AttitudeSpinning_ClassInited = (AttitudeSpinning::ClassInit(&AttitudeSpinning::staticType), true);

void AttitudeSpinning::ClassInit(Class* cls)
{

    cls->setName(NC_("Class", "AttitudeSpinning"));
    cls->setDesc(u8R"(自旋姿态 (Spinning))");
    cls->addToRegistry();
    cls->setParent<AttitudeProfile>();
    cls->setConstructor<AttitudeSpinning>();

    cls->addProperty("ReferenceAxes", aNewPropertyObject<AttitudeSpinning, Axes, &AttitudeSpinning::getReferenceAxes, &AttitudeSpinning::setReferenceAxes>());
    cls->addProperty("SpinRate", aNewPropertyDouble<AttitudeSpinning, &AttitudeSpinning::getSpinRate, &AttitudeSpinning::setSpinRate>());
    cls->addProperty("SpinOffset", aNewPropertyDouble<AttitudeSpinning, &AttitudeSpinning::getSpinOffset, &AttitudeSpinning::setSpinOffset>());
}

AST_NAMESPACE_END