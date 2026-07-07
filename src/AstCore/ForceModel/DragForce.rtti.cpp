#include "DragForce.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class DragForce::staticType;

static bool DragForce_ClassInited = (DragForce::ClassInit(&DragForce::staticType), true);

void DragForce::ClassInit(Class* cls)
{

    cls->setName(NC_("Class", "DragForce"));
    cls->setDesc(u8R"(大气阻力模型)");
    cls->addToRegistry();
    cls->setParent<ForceModel>();
    cls->setConstructor<DragForce>();

}

AST_NAMESPACE_END