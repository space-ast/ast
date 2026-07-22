#include "AndConstraint.hpp"

AST_NAMESPACE_BEGIN

Class AndConstraint::staticType;

static bool AndConstraint_ClassInited = (AndConstraint::ClassInit(&AndConstraint::staticType), true);

void AndConstraint::ClassInit(Class* cls)
{
    cls->setName(NC_("Class", "AndConstraint"));
    cls->addToRegistry();
    cls->setParent<AccessConstraint>();
    cls->setConstructor<AndConstraint>();
}

AST_NAMESPACE_END
