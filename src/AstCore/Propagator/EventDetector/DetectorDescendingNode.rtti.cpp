#include "DetectorDescendingNode.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class DetectorDescendingNode::staticType;

static bool DetectorDescendingNode_ClassInited = (DetectorDescendingNode::ClassInit(&DetectorDescendingNode::staticType), true);

void DetectorDescendingNode::ClassInit(Class* cls)
{

    cls->setName(N_("DetectorDescendingNode"));
    cls->addToRegistry();
    cls->setParent<EventDetector>();
    cls->setConstructor<DetectorDescendingNode>();

}

AST_NAMESPACE_END