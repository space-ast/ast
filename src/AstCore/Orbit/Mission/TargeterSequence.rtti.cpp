#include "TargeterSequence.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class TargeterSequence::staticType;

static bool TargeterSequence_ClassInited = (TargeterSequence::ClassInit(&TargeterSequence::staticType), true);

void TargeterSequence::ClassInit(Class* cls)
{

    cls->setName("TargeterSequence");
    cls->setDesc(u8R"(目标序列段，用于建模并求解轨迹规划问题，可以定义多个控制变量、约束条件、目标函数等，调用求解器求解满足约束和最优的轨迹)");
    cls->addToRegistry();
    cls->setParent<Sequence>();
    cls->setConstructor<TargeterSequence>();

}

AST_NAMESPACE_END