#include "LandingSiteCalculation.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class LandingSiteCalculation::staticType;

static bool LandingSiteCalculation_ClassInited = (LandingSiteCalculation::ClassInit(&LandingSiteCalculation::staticType), true);

void LandingSiteCalculation::ClassInit(Class* cls)
{

    cls->setName(N_("LandingSiteCalculation"));
    cls->setDesc(u8R"(着陆点计算量接口，定义计算与着陆点相关量的抽象接口)");
    cls->addToRegistry();
    cls->setParent<SegmentCalculation>();
    cls->setConstructor<LandingSiteCalculation>();

}

AST_NAMESPACE_END