#include "DistanceConstraint.hpp"

AST_NAMESPACE_BEGIN

Class DistanceConstraint::staticType;

static bool DistanceConstraint_ClassInited = (DistanceConstraint::ClassInit(&DistanceConstraint::staticType), true);

void DistanceConstraint::ClassInit(Class* cls)
{
    cls->setName(NC_("Class", "DistanceConstraint"));
    cls->addToRegistry();
    cls->setParent<AccessConstraint>();
    cls->setConstructor<DistanceConstraint>();
}

AST_NAMESPACE_END
