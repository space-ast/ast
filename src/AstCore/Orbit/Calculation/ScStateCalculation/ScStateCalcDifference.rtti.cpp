#include "ScStateCalcDifference.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class ScStateCalcDifference::staticType;

static bool ScStateCalcDifference_ClassInited = (ScStateCalcDifference::ClassInit(&ScStateCalcDifference::staticType), true);

void ScStateCalcDifference::ClassInit(Class* cls)
{

    cls->setName("ScStateCalcDifference");
    cls->addToRegistry();
    cls->setParent<ScStateCalculation>();
    cls->setConstructor<ScStateCalcDifference>();

}

AST_NAMESPACE_END