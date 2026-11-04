#include "ScStateCalcTimePastAscNode.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class ScStateCalcTimePastAscNode::staticType;

static bool ScStateCalcTimePastAscNode_ClassInited = (ScStateCalcTimePastAscNode::ClassInit(&ScStateCalcTimePastAscNode::staticType), true);

void ScStateCalcTimePastAscNode::ClassInit(Class* cls)
{

    cls->setName(NC_("Class", "ScStateCalcTimePastAscNode"));
    cls->setDesc(u8R"(过升交点后经过的时间)");
    cls->addToRegistry();
    cls->setParent<ScStateCalcFrameRelated>();
    cls->setConstructor<ScStateCalcTimePastAscNode>();

}

AST_NAMESPACE_END