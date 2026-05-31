#include "DetectorEpoch.hpp"

//  
// :

AST_NAMESPACE_BEGIN

Class DetectorEpoch::staticType;

static bool DetectorEpochInit = (DetectorEpoch::ClassInit(&DetectorEpoch::staticType), true);

void DetectorEpoch::ClassInit(Class* cls)
{
    cls->setName("DetectorEpoch");
    cls->addToRegistry();
    cls->setParent<EventDetector>();
    cls->setConstructor<DetectorEpoch>();
}

AST_NAMESPACE_END
