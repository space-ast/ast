#include "Command.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class Command::staticType;

static bool Command_ClassInited = (Command::ClassInit(&Command::staticType), true);

void Command::ClassInit(Class* cls)
{

    cls->setName("Command");
    cls->addToRegistry();
    cls->setParent<ObjectNamed>();
    cls->setConstructor<Command>();

}

AST_NAMESPACE_END