#include "ScStateCalcEccAnomaly.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class ScStateCalcEccAnomaly::staticType;

static bool ScStateCalcEccAnomaly_ClassInited = (ScStateCalcEccAnomaly::ClassInit(&ScStateCalcEccAnomaly::staticType), true);

void ScStateCalcEccAnomaly::ClassInit(Class* cls)
{

    cls->setName(NC_("Class", "ScStateCalcEccAnomaly"));
    cls->setDesc(u8R"(偏近点角)");
    cls->addToRegistry();
    cls->setParent<ScStateCalcBodyRelated>();
    cls->setConstructor<ScStateCalcEccAnomaly>();

}

AST_NAMESPACE_END