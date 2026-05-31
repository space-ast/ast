#include "AnalyzerVariable.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class AnalyzerVariable::staticType;

static bool AnalyzerVariable_ClassInited = (AnalyzerVariable::ClassInit(&AnalyzerVariable::staticType), true);

void AnalyzerVariable::ClassInit(Class* cls)
{

    cls->setName("AnalyzerVariable");
    cls->addToRegistry();
    cls->setParent<ObjectNamed>();
    cls->setConstructor<AnalyzerVariable>();

}

AST_NAMESPACE_END