#include "DetectorAlwaysTripped.hpp"

//  
// :

AST_NAMESPACE_BEGIN

Class DetectorAlwaysTripped::staticType;

static bool DetectorAlwaysTrippedInit = (DetectorAlwaysTripped::ClassInit(&DetectorAlwaysTripped::staticType), true);

void DetectorAlwaysTripped::ClassInit(Class* cls)
{
    cls->setName("DetectorAlwaysTripped");
    cls->addToRegistry();
    cls->setParent<EventDetector>();
    cls->setConstructor<DetectorAlwaysTripped>();
}

AST_NAMESPACE_END
