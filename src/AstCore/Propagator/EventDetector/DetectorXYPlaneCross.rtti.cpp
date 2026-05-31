#include "DetectorXYPlaneCross.hpp"

//  
// :

AST_NAMESPACE_BEGIN

Class DetectorXYPlaneCross::staticType;

static bool DetectorXYPlaneCrossInit = (DetectorXYPlaneCross::ClassInit(&DetectorXYPlaneCross::staticType), true);

void DetectorXYPlaneCross::ClassInit(Class* cls)
{
    cls->setName("DetectorXYPlaneCross");
    cls->addToRegistry();
    cls->setParent<EventDetector>();
    cls->setConstructor<DetectorXYPlaneCross>();
}

AST_NAMESPACE_END
