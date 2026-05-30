#include "DetectorDeltaV.hpp"

//  
// :

AST_NAMESPACE_BEGIN

Class DetectorDeltaV::staticType;

static bool DetectorDeltaVInit = (DetectorDeltaV::ClassInit(&DetectorDeltaV::staticType), true);

void DetectorDeltaV::ClassInit(Class* cls)
{
    cls->setName("DetectorDeltaV");
    cls->addToRegistry();
    cls->setParent<EventDetector>();
    cls->setConstructor<DetectorDeltaV>();
}

AST_NAMESPACE_END
