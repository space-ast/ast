#include "ScStateCalcAltitudeOfApoapsis.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class ScStateCalcAltitudeOfApoapsis::staticType;

static bool ScStateCalcAltitudeOfApoapsis_ClassInited = (ScStateCalcAltitudeOfApoapsis::ClassInit(&ScStateCalcAltitudeOfApoapsis::staticType), true);

void ScStateCalcAltitudeOfApoapsis::ClassInit(Class* cls)
{

    cls->setName(NC_("Class", "ScStateCalcAltitudeOfApoapsis"));
    cls->setDesc(u8R"(远地点高度)");
    cls->addToRegistry();
    cls->setParent<ScStateCalcBodyRelated>();
    cls->setConstructor<ScStateCalcAltitudeOfApoapsis>();

}

AST_NAMESPACE_END