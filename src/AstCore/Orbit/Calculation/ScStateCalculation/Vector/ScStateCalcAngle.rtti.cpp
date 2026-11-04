#include "ScStateCalcAngle.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class ScStateCalcAngle::staticType;

static bool ScStateCalcAngle_ClassInited = (ScStateCalcAngle::ClassInit(&ScStateCalcAngle::staticType), true);

void ScStateCalcAngle::ClassInit(Class* cls)
{

    cls->setName(NC_("Class", "ScStateCalcAngle"));
    cls->setDesc(u8R"(两向量夹角)");
    cls->addToRegistry();
    cls->setParent<ScStateCalculation>();
    cls->setConstructor<ScStateCalcAngle>();

    cls->addProperty("Vector1", aNewPropertyObject<ScStateCalcAngle, Vector, &ScStateCalcAngle::getVector1, &ScStateCalcAngle::setVector1>());
    cls->addProperty("Vector2", aNewPropertyObject<ScStateCalcAngle, Vector, &ScStateCalcAngle::getVector2, &ScStateCalcAngle::setVector2>());
}

AST_NAMESPACE_END