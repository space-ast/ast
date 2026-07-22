#include "AccessEvaluator.hpp"

AST_NAMESPACE_BEGIN

Class AccessEvaluator::staticType;

static bool AccessEvaluator_ClassInited = (AccessEvaluator::ClassInit(&AccessEvaluator::staticType), true);

void AccessEvaluator::ClassInit(Class* cls)
{
    cls->setName(NC_("Class", "AccessEvaluator"));
    cls->addToRegistry();
    cls->setParent<ObjectNamed>();
    cls->setConstructor<AccessEvaluator>();
}

AST_NAMESPACE_END
