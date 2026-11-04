#include "ScStateCalcLongDriftRate.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class ScStateCalcLongDriftRate::staticType;

static bool ScStateCalcLongDriftRate_ClassInited = (ScStateCalcLongDriftRate::ClassInit(&ScStateCalcLongDriftRate::staticType), true);

void ScStateCalcLongDriftRate::ClassInit(Class* cls)
{

    cls->setName(NC_("Class", "ScStateCalcLongDriftRate"));
    cls->setDesc(u8R"(经度漂移率)");
    cls->addToRegistry();
    cls->setParent<ScStateCalcBodyRelated>();
    cls->setConstructor<ScStateCalcLongDriftRate>();

}

AST_NAMESPACE_END
