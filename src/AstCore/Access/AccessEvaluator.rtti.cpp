#include "AccessEvaluator.hpp"

#include "AstCore/ReflectAPI.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class AccessEvaluator::staticType;

Class* AccessEvaluator::StaticType()
{
    return &staticType;
}

Class* AccessEvaluator::getType() const
{
    return &staticType;
}

static bool AccessEvaluator_ClassInited = (AccessEvaluator::ClassInit(&AccessEvaluator::staticType), true);

void AccessEvaluator::ClassInit(Class* cls)
{

    cls->setName(NC_("Class", "AccessEvaluator"));
    cls->setDesc(u8R"(访问评估器)");
    cls->addToRegistry();
    cls->setParent<ObjectNamed>();
    cls->setConstructor<AccessEvaluator>();

}

AST_NAMESPACE_END