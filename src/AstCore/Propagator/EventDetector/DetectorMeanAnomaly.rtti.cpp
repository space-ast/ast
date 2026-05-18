#include "DetectorMeanAnomaly.hpp"

//  
// :

AST_NAMESPACE_BEGIN

Class DetectorMeanAnomaly::staticType;

static bool DetectorMeanAnomalyInit = (DetectorMeanAnomaly::ClassInit(&DetectorMeanAnomaly::staticType), true);

void DetectorMeanAnomaly::ClassInit(Class* cls)
{
    cls->setName("DetectorMeanAnomaly");
    cls->addToRegistry();
    cls->setParent<EventDetector>();
    cls->setConstructor<DetectorMeanAnomaly>();
}

AST_NAMESPACE_END
