#include "BodyObstructionConstraint.hpp"

AST_NAMESPACE_BEGIN

Class BodyObstructionConstraint::staticType;

static bool BodyObstructionConstraint_ClassInited = (BodyObstructionConstraint::ClassInit(&BodyObstructionConstraint::staticType), true);

void BodyObstructionConstraint::ClassInit(Class* cls)
{
    cls->setName(NC_("Class", "BodyObstructionConstraint"));
    cls->addToRegistry();
    cls->setParent<AccessConstraint>();
    cls->setConstructor<BodyObstructionConstraint>();
}

AST_NAMESPACE_END
