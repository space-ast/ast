#include "FieldOfViewConstraint.hpp"

AST_NAMESPACE_BEGIN

Class FieldOfViewConstraint::staticType;

static bool FieldOfViewConstraint_ClassInited = (FieldOfViewConstraint::ClassInit(&FieldOfViewConstraint::staticType), true);

void FieldOfViewConstraint::ClassInit(Class* cls)
{
    cls->setName(NC_("Class", "FieldOfViewConstraint"));
    cls->addToRegistry();
    cls->setParent<AccessConstraint>();
    cls->setConstructor<FieldOfViewConstraint>();
}

AST_NAMESPACE_END
