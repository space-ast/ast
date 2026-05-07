#include "Target.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class Target::staticType;

static bool Target_ClassInited = (Target::ClassInit(&Target::staticType), true);

void Target::ClassInit(Class* cls)
{

    cls->setName("Target");
    cls->setDesc(u8R"(目标对象)");
    cls->addToRegistry();
    cls->setParent<Facility>();
    cls->setConstructor<Target>();

}

AST_NAMESPACE_END