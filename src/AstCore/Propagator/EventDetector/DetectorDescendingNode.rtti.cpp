#include "DetectorDescendingNode.hpp"

//  
// :

AST_NAMESPACE_BEGIN

Class DetectorDescendingNode::staticType;

static bool DetectorDescendingNodeInit = (DetectorDescendingNode::ClassInit(&DetectorDescendingNode::staticType), true);

void DetectorDescendingNode::ClassInit(Class* cls)
{
    cls->setName("DetectorDescendingNode");
    cls->addToRegistry();
    cls->setParent<EventDetector>();
    cls->setConstructor<DetectorDescendingNode>();
}

AST_NAMESPACE_END
