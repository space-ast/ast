#include "MainSequence.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class MainSequence::staticType;

static bool MainSequence_ClassInited = (MainSequence::ClassInit(&MainSequence::staticType), true);

void MainSequence::ClassInit(Class* cls)
{

    cls->setName(N_("MainSequence"));
    cls->addToRegistry();
    cls->setParent<Sequence>();
    cls->setConstructor<MainSequence>();

}

AST_NAMESPACE_END