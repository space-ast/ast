#include "ScStateCalcPointRelated.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class ScStateCalcPointRelated::staticType;

static bool ScStateCalcPointRelated_ClassInited = (ScStateCalcPointRelated::ClassInit(&ScStateCalcPointRelated::staticType), true);

void ScStateCalcPointRelated::ClassInit(Class* cls)
{

    cls->setName("ScStateCalcPointRelated");
    cls->addToRegistry();
    cls->setParent<ScStateCalculation>();
    cls->setConstructor<ScStateCalcPointRelated>();

    cls->addProperty("point", aNewPropertyObject<ScStateCalcPointRelated, Point, &ScStateCalcPointRelated::point, &ScStateCalcPointRelated::setPoint>());
}

AST_NAMESPACE_END