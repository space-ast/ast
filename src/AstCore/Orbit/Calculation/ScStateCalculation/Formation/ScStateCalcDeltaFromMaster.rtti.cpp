#include "ScStateCalcDeltaFromMaster.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class ScStateCalcDeltaFromMaster::staticType;

static bool ScStateCalcDeltaFromMaster_ClassInited = (ScStateCalcDeltaFromMaster::ClassInit(&ScStateCalcDeltaFromMaster::staticType), true);

void ScStateCalcDeltaFromMaster::ClassInit(Class* cls)
{

    cls->setName(NC_("Class", "ScStateCalcDeltaFromMaster"));
    cls->setDesc(u8R"(相对主航天器的差值计算量)");
    cls->addToRegistry();
    cls->setParent<ScStateCalculation>();
    cls->setConstructor<ScStateCalcDeltaFromMaster>();

    cls->addProperty("calculation", aNewPropertyObject<ScStateCalcDeltaFromMaster, ScStateCalculation, &ScStateCalcDeltaFromMaster::calculation, &ScStateCalcDeltaFromMaster::setCalculation>());
    cls->addProperty("reference", aNewPropertyObject<ScStateCalcDeltaFromMaster, Point, &ScStateCalcDeltaFromMaster::reference, &ScStateCalcDeltaFromMaster::setReference>());
}

AST_NAMESPACE_END