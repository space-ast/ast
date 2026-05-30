#include "DetectorLighting.hpp"

//  
// :

AST_NAMESPACE_BEGIN

Class DetectorLighting::staticType;

static bool DetectorLightingInit = (DetectorLighting::ClassInit(&DetectorLighting::staticType), true);

void DetectorLighting::ClassInit(Class* cls)
{
    cls->setName("DetectorLighting");
    cls->addToRegistry();
    cls->setParent<EventDetector>();
    cls->setConstructor<DetectorLighting>();
}

AST_NAMESPACE_END
