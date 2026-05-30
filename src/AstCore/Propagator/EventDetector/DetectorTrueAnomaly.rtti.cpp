#include "DetectorTrueAnomaly.hpp"

//  
// :

AST_NAMESPACE_BEGIN

Class DetectorTrueAnomaly::staticType;

static bool DetectorTrueAnomalyInit = (DetectorTrueAnomaly::ClassInit(&DetectorTrueAnomaly::staticType), true);

void DetectorTrueAnomaly::ClassInit(Class* cls)
{
    cls->setName("DetectorTrueAnomaly");
    cls->addToRegistry();
    cls->setParent<EventDetector>();
    cls->setConstructor<DetectorTrueAnomaly>();
}

AST_NAMESPACE_END
