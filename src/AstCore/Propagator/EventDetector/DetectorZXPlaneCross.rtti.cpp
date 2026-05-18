#include "DetectorZXPlaneCross.hpp"

//  
// :

AST_NAMESPACE_BEGIN

Class DetectorZXPlaneCross::staticType;

static bool DetectorZXPlaneCrossInit = (DetectorZXPlaneCross::ClassInit(&DetectorZXPlaneCross::staticType), true);

void DetectorZXPlaneCross::ClassInit(Class* cls)
{
    cls->setName("DetectorZXPlaneCross");
    cls->addToRegistry();
    cls->setParent<EventDetector>();
    cls->setConstructor<DetectorZXPlaneCross>();
}

AST_NAMESPACE_END
