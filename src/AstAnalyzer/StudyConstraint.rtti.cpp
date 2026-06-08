#include "StudyConstraint.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class StudyConstraint::staticType;

static bool StudyConstraint_ClassInited = (StudyConstraint::ClassInit(&StudyConstraint::staticType), true);

void StudyConstraint::ClassInit(Class* cls)
{

    cls->setName("StudyConstraint");
    cls->addToRegistry();
    cls->setParent<ObjectNamed>();
    cls->setConstructor<StudyConstraint>();

}

AST_NAMESPACE_END