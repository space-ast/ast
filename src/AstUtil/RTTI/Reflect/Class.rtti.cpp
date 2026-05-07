#include "Class.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class Class::staticType;

static bool Class_ClassInited = (Class::ClassInit(&Class::staticType), true);

void Class::ClassInit(Class* cls)
{

    cls->setName("Class");
    cls->setDesc(u8R"(类元信息)");
    cls->addToRegistry();
    cls->setParent(nullptr);
    cls->setConstructor<Class>();

}

AST_NAMESPACE_END