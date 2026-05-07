#include "Aircraft.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class Aircraft::staticType;

static bool Aircraft_ClassInited = (Aircraft::ClassInit(&Aircraft::staticType), true);

void Aircraft::ClassInit(Class* cls)
{

    cls->setName("Aircraft");
    cls->setDesc(u8R"(航空器对象)");
    cls->addToRegistry();
    cls->setParent<Mover>();
    cls->setConstructor<Aircraft>();

}

AST_NAMESPACE_END