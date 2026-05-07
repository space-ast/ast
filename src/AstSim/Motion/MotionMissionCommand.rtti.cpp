#include "MotionMissionCommand.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class MotionMissionCommand::staticType;

static bool MotionMissionCommand_ClassInited = (MotionMissionCommand::ClassInit(&MotionMissionCommand::staticType), true);

void MotionMissionCommand::ClassInit(Class* cls)
{

    cls->setName("MotionMissionCommand");
    cls->setDesc(u8R"(任务命令运动模型，定义并协调一系列轨道机动任务（类似 STK Astrogator 中的任务控制序列MCS）)");
    cls->addToRegistry();
    cls->setParent<MotionProfile>();
    cls->setConstructor<MotionMissionCommand>();

}

AST_NAMESPACE_END