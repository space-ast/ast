#include "HPOP.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class HPOP::staticType;

static bool HPOP_ClassInited = (HPOP::ClassInit(&HPOP::staticType), true);

void HPOP::ClassInit(Class* cls)
{

    cls->setName("HPOP");
    cls->setDesc(u8R"(高精度轨道预报器)");
    cls->addToRegistry();
    cls->setParent<ObjectNamed>();
    cls->setConstructor<HPOP>();

}

AST_NAMESPACE_END