#include "InitialState.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class InitialState::staticType;

static bool InitialState_ClassInited = (InitialState::ClassInit(&InitialState::staticType), true);

void InitialState::ClassInit(Class* cls)
{

    cls->setName("InitialState");
    cls->setDesc(u8R"(初始轨道段，用于建模任务序列(MissionCommand)中的初始状态任务)");
    cls->addToRegistry();
    cls->setParent<Segment>();
    cls->setConstructor<InitialState>();

}

AST_NAMESPACE_END