#include "DetectorAscendingNode.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class DetectorAscendingNode::staticType;

static bool DetectorAscendingNode_ClassInited = (DetectorAscendingNode::ClassInit(&DetectorAscendingNode::staticType), true);

void DetectorAscendingNode::ClassInit(Class* cls)
{

    cls->setName(NC_("Class", "DetectorAscendingNode"));
    cls->addToRegistry();
    cls->setParent<EventDetector>();
    cls->setConstructor<DetectorAscendingNode>();

}

AST_NAMESPACE_END