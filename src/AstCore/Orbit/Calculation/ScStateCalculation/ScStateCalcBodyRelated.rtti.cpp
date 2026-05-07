#include "ScStateCalcBodyRelated.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class ScStateCalcBodyRelated::staticType;

static bool ScStateCalcBodyRelated_ClassInited = (ScStateCalcBodyRelated::ClassInit(&ScStateCalcBodyRelated::staticType), true);

void ScStateCalcBodyRelated::ClassInit(Class* cls)
{

    cls->setName("ScStateCalcBodyRelated");
    cls->addToRegistry();
    cls->setParent<ScStateCalculation>();
    cls->setConstructor<ScStateCalcBodyRelated>();

    cls->addProperty("body", aNewPropertyObject<ScStateCalcBodyRelated, Body, &ScStateCalcBodyRelated::body, &ScStateCalcBodyRelated::setBody>());
}

AST_NAMESPACE_END