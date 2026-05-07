#include "Chain.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class Chain::staticType;

static bool Chain_ClassInited = (Chain::ClassInit(&Chain::staticType), true);

void Chain::ClassInit(Class* cls)
{

    cls->setName("Chain");
    cls->setDesc(u8R"(链对象)");
    cls->addToRegistry();
    cls->setParent<ObjectNamed>();
    cls->setConstructor<Chain>();

}

AST_NAMESPACE_END