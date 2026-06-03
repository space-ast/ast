#include "BasicAnalyzer.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class BasicAnalyzer::staticType;

static bool BasicAnalyzer_ClassInited = (BasicAnalyzer::ClassInit(&BasicAnalyzer::staticType), true);

void BasicAnalyzer::ClassInit(Class* cls)
{

    cls->setName("BasicAnalyzer");
    cls->addToRegistry();
    cls->setParent<ObjectNamed>();
    cls->setConstructor<BasicAnalyzer>();

}

AST_NAMESPACE_END
