#include "MissionCommand.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class MissionCommand::staticType;

static bool MissionCommand_ClassInited = (MissionCommand::ClassInit(&MissionCommand::staticType), true);

void MissionCommand::ClassInit(Class* cls)
{

    cls->setName("MissionCommand");
    cls->setDesc(u8R"(任务命令（虚基类），用于定义任务序列中的任务命令，例如初始状态、轨道机动、轨道预报等)");
    cls->addToRegistry();
    cls->setParent<Command>();
    cls->setConstructor<MissionCommand>();

}

AST_NAMESPACE_END