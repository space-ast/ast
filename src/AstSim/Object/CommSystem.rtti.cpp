#include "CommSystem.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class CommSystem::staticType;

static bool CommSystem_ClassInited = (CommSystem::ClassInit(&CommSystem::staticType), true);

void CommSystem::ClassInit(Class* cls)
{

    cls->setName("CommSystem");
    cls->setDesc(u8R"(通信系统对象)");
    cls->addToRegistry();
    cls->setParent<ObjectNamed>();
    cls->setConstructor<CommSystem>();

}

AST_NAMESPACE_END