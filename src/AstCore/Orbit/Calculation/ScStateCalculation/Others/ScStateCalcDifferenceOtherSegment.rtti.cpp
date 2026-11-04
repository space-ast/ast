#include "ScStateCalcDifferenceOtherSegment.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class ScStateCalcDifferenceOtherSegment::staticType;

static bool ScStateCalcDifferenceOtherSegment_ClassInited = (ScStateCalcDifferenceOtherSegment::ClassInit(&ScStateCalcDifferenceOtherSegment::staticType), true);

void ScStateCalcDifferenceOtherSegment::ClassInit(Class* cls)
{

    cls->setName(NC_("Class", "ScStateCalcDifferenceOtherSegment"));
    cls->setDesc(u8R"(相对另一轨道段的差值计算量)");
    cls->addToRegistry();
    cls->setParent<ScStateCalculation>();
    cls->setConstructor<ScStateCalcDifferenceOtherSegment>();

    cls->addProperty("calculation", aNewPropertyObject<ScStateCalcDifferenceOtherSegment, ScStateCalculation, &ScStateCalcDifferenceOtherSegment::calculation, &ScStateCalcDifferenceOtherSegment::setCalculation>());
    cls->addProperty("otherSegment", aNewPropertyObject<ScStateCalcDifferenceOtherSegment, Segment, &ScStateCalcDifferenceOtherSegment::otherSegment, &ScStateCalcDifferenceOtherSegment::setOtherSegment>());
}

AST_NAMESPACE_END