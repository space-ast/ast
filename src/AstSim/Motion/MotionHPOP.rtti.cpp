#include "MotionHPOP.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class MotionHPOP::staticType;

static bool MotionHPOP_ClassInited = (MotionHPOP::ClassInit(&MotionHPOP::staticType), true);

void MotionHPOP::ClassInit(Class* cls)
{

    cls->setName(NC_("Class", "MotionHPOP"));
    cls->addToRegistry();
    cls->setParent<MotionBasic>();
    cls->setConstructor<MotionHPOP>();

}

AST_NAMESPACE_END