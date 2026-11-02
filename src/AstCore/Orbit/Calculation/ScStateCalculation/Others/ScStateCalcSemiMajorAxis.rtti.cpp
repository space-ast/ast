#include "ScStateCalcSemiMajorAxis.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class ScStateCalcSemiMajorAxis::staticType;

static bool ScStateCalcSemiMajorAxis_ClassInited = (ScStateCalcSemiMajorAxis::ClassInit(&ScStateCalcSemiMajorAxis::staticType), true);

void ScStateCalcSemiMajorAxis::ClassInit(Class* cls)
{

    cls->setName(NC_("Class", "ScStateCalcSemiMajorAxis"));
    cls->setDesc(u8R"(半长轴)");
    cls->addToRegistry();
    cls->setParent<ScStateCalcBodyRelated>();
    cls->setConstructor<ScStateCalcSemiMajorAxis>();

}

AST_NAMESPACE_END