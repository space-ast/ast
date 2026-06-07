#include "LandingSiteCalcSunAzimuth.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class LandingSiteCalcSunAzimuth::staticType;

static bool LandingSiteCalcSunAzimuth_ClassInited = (LandingSiteCalcSunAzimuth::ClassInit(&LandingSiteCalcSunAzimuth::staticType), true);

void LandingSiteCalcSunAzimuth::ClassInit(Class* cls)
{

    cls->setName(N_("LandingSiteCalcSunAzimuth"));
    cls->addToRegistry();
    cls->setParent<SegmentCalculation>();
    cls->setConstructor<LandingSiteCalcSunAzimuth>();

}

AST_NAMESPACE_END