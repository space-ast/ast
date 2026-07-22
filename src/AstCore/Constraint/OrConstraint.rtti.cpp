#include "OrConstraint.hpp"

AST_NAMESPACE_BEGIN

Class OrConstraint::staticType;

static bool OrConstraint_ClassInited = (OrConstraint::ClassInit(&OrConstraint::staticType), true);

void OrConstraint::ClassInit(Class* cls)
{
    cls->setName(NC_("Class", "OrConstraint"));
    cls->addToRegistry();
    cls->setParent<AccessConstraint>();
    cls->setConstructor<OrConstraint>();
}

AST_NAMESPACE_END
