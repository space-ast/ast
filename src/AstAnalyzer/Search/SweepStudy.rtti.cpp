#include "SweepStudy.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class SweepStudy::staticType;

static bool SweepStudy_ClassInited = (SweepStudy::ClassInit(&SweepStudy::staticType), true);

void SweepStudy::ClassInit(Class* cls)
{

    cls->setName("SweepStudy");
    cls->addToRegistry();
    cls->setParent<Command>();
    cls->setConstructor<SweepStudy>();

}

AST_NAMESPACE_END