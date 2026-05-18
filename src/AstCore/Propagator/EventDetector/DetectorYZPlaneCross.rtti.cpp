#include "DetectorYZPlaneCross.hpp"

//  
// :

AST_NAMESPACE_BEGIN

Class DetectorYZPlaneCross::staticType;

static bool DetectorYZPlaneCrossInit = (DetectorYZPlaneCross::ClassInit(&DetectorYZPlaneCross::staticType), true);

void DetectorYZPlaneCross::ClassInit(Class* cls)
{
    cls->setName("DetectorYZPlaneCross");
    cls->addToRegistry();
    cls->setParent<EventDetector>();
    cls->setConstructor<DetectorYZPlaneCross>();
}

AST_NAMESPACE_END
