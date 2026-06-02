#include "LandingSiteCalcSunElevation.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class LandingSiteCalcSunElevation::staticType;

static bool LandingSiteCalcSunElevation_ClassInited = (LandingSiteCalcSunElevation::ClassInit(&LandingSiteCalcSunElevation::staticType), true);

void LandingSiteCalcSunElevation::ClassInit(Class* cls)
{

    cls->setName("LandingSiteCalcSunElevation");
    cls->setDesc(u8R"(计算着陆点太阳高度角)");
    cls->addToRegistry();
    cls->setParent<LandingSiteCalculation>();
    cls->setConstructor<LandingSiteCalcSunElevation>();

}

AST_NAMESPACE_END