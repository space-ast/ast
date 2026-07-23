#include "AccessConstraint.hpp"

// RTTI 注册代码

AST_NAMESPACE_BEGIN

Class AccessConstraint::staticType;

static bool AccessConstraint_ClassInited = (AccessConstraint::ClassInit(&AccessConstraint::staticType), true);

void AccessConstraint::ClassInit(Class* cls)
{
    cls->setName(NC_("Class", "AccessConstraint"));
    cls->addToRegistry();
    cls->setParent<ObjectNamed>();
}

AST_NAMESPACE_END
