#include "ScStateCalcFrameRelated.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class ScStateCalcFrameRelated::staticType;

static bool ScStateCalcFrameRelated_ClassInited = (ScStateCalcFrameRelated::ClassInit(&ScStateCalcFrameRelated::staticType), true);

void ScStateCalcFrameRelated::ClassInit(Class* cls)
{

    cls->setName("ScStateCalcFrameRelated");
    cls->addToRegistry();
    cls->setParent<ScStateCalculation>();
    cls->setConstructor<ScStateCalcFrameRelated>();

    cls->addProperty("frame", aNewPropertyObject<ScStateCalcFrameRelated, Frame, &ScStateCalcFrameRelated::frame, &ScStateCalcFrameRelated::setFrame>());
}

AST_NAMESPACE_END