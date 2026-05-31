#include "DetectorCartesian.hpp"

//  
// :

AST_NAMESPACE_BEGIN

Class DetectorCartesian::staticType;

static bool DetectorCartesianInit = (DetectorCartesian::ClassInit(&DetectorCartesian::staticType), true);

void DetectorCartesian::ClassInit(Class* cls)
{
    cls->setName("DetectorCartesian");
    cls->addToRegistry();
    cls->setParent<EventDetector>();
    cls->setConstructor<DetectorCartesian>();
}

AST_NAMESPACE_END
