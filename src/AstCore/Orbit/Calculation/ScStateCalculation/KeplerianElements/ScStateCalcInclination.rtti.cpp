#include "ScStateCalcInclination.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class ScStateCalcInclination::staticType;

static bool ScStateCalcInclination_ClassInited = (ScStateCalcInclination::ClassInit(&ScStateCalcInclination::staticType), true);

void ScStateCalcInclination::ClassInit(Class* cls)
{

    cls->setName(NC_("Class", "ScStateCalcInclination"));
    cls->setDesc(u8R"(轨道倾角)");
    cls->addToRegistry();
    cls->setParent<ScStateCalcFrameRelated>();
    cls->setConstructor<ScStateCalcInclination>();

}

AST_NAMESPACE_END