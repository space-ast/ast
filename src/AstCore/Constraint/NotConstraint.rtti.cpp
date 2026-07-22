#include "NotConstraint.hpp"

AST_NAMESPACE_BEGIN

Class NotConstraint::staticType;

static bool NotConstraint_ClassInited = (NotConstraint::ClassInit(&NotConstraint::staticType), true);

void NotConstraint::ClassInit(Class* cls)
{
    cls->setName(NC_("Class", "NotConstraint"));
    cls->addToRegistry();
    cls->setParent<AccessConstraint>();
    cls->setConstructor<NotConstraint>();
}

AST_NAMESPACE_END
