#include "Stop.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class Stop::staticType;

static bool Stop_ClassInited = (Stop::ClassInit(&Stop::staticType), true);

void Stop::ClassInit(Class* cls)
{

    cls->setName("Stop");
    cls->setDesc(u8R"(停止段，用于停止任务序列的执行)");
    cls->addToRegistry();
    cls->setParent<MissionCommand>();
    cls->setConstructor<Stop>();

}

AST_NAMESPACE_END