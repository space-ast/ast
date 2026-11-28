#include "ScStateCalcBodyRelated.hpp"

#include "AstCore/ReflectAPI.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class ScStateCalcBodyRelated::staticType;

Class* ScStateCalcBodyRelated::StaticType()
{
    return &staticType;
}

Class* ScStateCalcBodyRelated::getType() const
{
    return &staticType;
}

static bool ScStateCalcBodyRelated_ClassInited = (ScStateCalcBodyRelated::ClassInit(&ScStateCalcBodyRelated::staticType), true);

void ScStateCalcBodyRelated::ClassInit(Class* cls)
{

    cls->setName(NC_("Class", "ScStateCalcBodyRelated"));
    cls->addToRegistry();
    cls->setParent<ScStateCalculation>();
    cls->setConstructor<ScStateCalcBodyRelated>();

    cls->addProperty("body", aNewPropertyObject<ScStateCalcBodyRelated, Body, &ScStateCalcBodyRelated::body, &ScStateCalcBodyRelated::setBody>());
}

AST_NAMESPACE_END