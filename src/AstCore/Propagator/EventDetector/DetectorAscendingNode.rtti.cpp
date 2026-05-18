#include "DetectorAscendingNode.hpp"

//  
// :

AST_NAMESPACE_BEGIN

Class DetectorAscendingNode::staticType;

static bool DetectorAscendingNodeInit = (DetectorAscendingNode::ClassInit(&DetectorAscendingNode::staticType), true);

void DetectorAscendingNode::ClassInit(Class* cls)
{
    cls->setName("DetectorAscendingNode");
    cls->addToRegistry();
    cls->setParent<EventDetector>();
    cls->setConstructor<DetectorAscendingNode>();
}

AST_NAMESPACE_END
