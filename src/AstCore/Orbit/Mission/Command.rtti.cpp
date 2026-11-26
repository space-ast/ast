#include "Command.hpp"

#include "AstCore/ReflectAPI.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class Command::staticType;

Class* Command::StaticType()
{
    return &staticType;
}

Class* Command::getType() const
{
    return &staticType;
}

static bool Command_ClassInited = (Command::ClassInit(&Command::staticType), true);

void Command::ClassInit(Class* cls)
{

    cls->setName(NC_("Class", "Command"));
    cls->setDesc(u8R"(命令（虚基类），用于定义任务序列中的命令，例如初始状态、轨道机动、轨道预报等)");
    cls->addToRegistry();
    cls->setParent<ObjectNamed>();
    cls->setConstructor<Command>();

}

AST_NAMESPACE_END