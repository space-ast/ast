#include "DetectorAccess.hpp"

//  
// :

AST_NAMESPACE_BEGIN

Class DetectorAccess::staticType;

static bool DetectorAccessInit = (DetectorAccess::ClassInit(&DetectorAccess::staticType), true);

void DetectorAccess::ClassInit(Class* cls)
{
    cls->setName("DetectorAccess");
    cls->addToRegistry();
    cls->setParent<EventDetector>();
    cls->setConstructor<DetectorAccess>();
}

AST_NAMESPACE_END
