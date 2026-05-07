#include "ScStateCalcEccentricity.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class ScStateCalcEccentricity::staticType;

static bool ScStateCalcEccentricity_ClassInited = (ScStateCalcEccentricity::ClassInit(&ScStateCalcEccentricity::staticType), true);

void ScStateCalcEccentricity::ClassInit(Class* cls)
{

    cls->setName("ScStateCalcEccentricity");
    cls->addToRegistry();
    cls->setParent<ScStateCalcBodyRelated>();
    cls->setConstructor<ScStateCalcEccentricity>();

}

AST_NAMESPACE_END