#include "DetectorAltitude.hpp"

//  
// :

AST_NAMESPACE_BEGIN

Class DetectorAltitude::staticType;

static bool DetectorAltitudeInit = (DetectorAltitude::ClassInit(&DetectorAltitude::staticType), true);

void DetectorAltitude::ClassInit(Class* cls)
{
    cls->setName("DetectorAltitude");
    cls->addToRegistry();
    cls->setParent<EventDetector>();
    cls->setConstructor<DetectorAltitude>();
}

AST_NAMESPACE_END
