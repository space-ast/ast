#include "ScStateCalculation.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class ScStateCalculation::staticType;

static bool ScStateCalculation_ClassInited = (ScStateCalculation::ClassInit(&ScStateCalculation::staticType), true);

void ScStateCalculation::ClassInit(Class* cls)
{

    cls->setName("ScStateCalculation");
    cls->addToRegistry();
    cls->setParent<ObjectCalculation>();
    cls->setConstructor<ScStateCalculation>();

}

AST_NAMESPACE_END