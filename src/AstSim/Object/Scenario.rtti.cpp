#include "Scenario.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class Scenario::staticType;

static bool Scenario_ClassInited = (Scenario::ClassInit(&Scenario::staticType), true);

void Scenario::ClassInit(Class* cls)
{

    cls->setName("Scenario");
    cls->setDesc(u8R"(场景类)");
    cls->addToRegistry();
    cls->setParent<Object>();
    cls->setConstructor<Scenario>();

}

AST_NAMESPACE_END