#include "AccessStepper.hpp"

AST_NAMESPACE_BEGIN

Class AccessStepper::staticType;

static bool AccessStepper_ClassInited = (AccessStepper::ClassInit(&AccessStepper::staticType), true);

void AccessStepper::ClassInit(Class* cls)
{
    cls->setName(NC_("Class", "AccessStepper"));
    cls->addToRegistry();
    cls->setParent<ObjectNamed>();
}

AST_NAMESPACE_END
