#include "StudyVariable.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class StudyVariable::staticType;

static bool StudyVariable_ClassInited = (StudyVariable::ClassInit(&StudyVariable::staticType), true);

void StudyVariable::ClassInit(Class* cls)
{

    cls->setName("StudyVariable");
    cls->addToRegistry();
    cls->setParent<ObjectNamed>();
    cls->setConstructor<StudyVariable>();

}

AST_NAMESPACE_END