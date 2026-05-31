#include "AnalyzerConstraint.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class AnalyzerConstraint::staticType;

static bool AnalyzerConstraint_ClassInited = (AnalyzerConstraint::ClassInit(&AnalyzerConstraint::staticType), true);

void AnalyzerConstraint::ClassInit(Class* cls)
{

    cls->setName("AnalyzerConstraint");
    cls->addToRegistry();
    cls->setParent<ObjectNamed>();
    cls->setConstructor<AnalyzerConstraint>();

}

AST_NAMESPACE_END