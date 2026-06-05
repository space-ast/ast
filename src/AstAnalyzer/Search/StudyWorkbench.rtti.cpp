#include "StudyWorkbench.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class StudyWorkbench::staticType;

static bool StudyWorkbench_ClassInited = (StudyWorkbench::ClassInit(&StudyWorkbench::staticType), true);

void StudyWorkbench::ClassInit(Class* cls)
{

    cls->setName("StudyWorkbench");
    cls->addToRegistry();
    cls->setParent<ObjectNamed>();
    cls->setConstructor<StudyWorkbench>();

}

AST_NAMESPACE_END
