#include "FixedStepStepper.hpp"

AST_NAMESPACE_BEGIN

Class FixedStepStepper::staticType;

static bool FixedStepStepper_ClassInited = (FixedStepStepper::ClassInit(&FixedStepStepper::staticType), true);

void FixedStepStepper::ClassInit(Class* cls)
{
    cls->setName(NC_("Class", "FixedStepStepper"));
    cls->addToRegistry();
    cls->setParent<AccessStepper>();
    cls->setConstructor<FixedStepStepper>();
}

AST_NAMESPACE_END
