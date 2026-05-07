#include "Propagate.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class Propagate::staticType;

static bool Propagate_ClassInited = (Propagate::ClassInit(&Propagate::staticType), true);

void Propagate::ClassInit(Class* cls)
{

    cls->setName("Propagate");
    cls->setDesc(u8R"(预报段，用于建模任务序列(MissionCommand)中的轨道预报)");
    cls->addToRegistry();
    cls->setParent<Segment>();
    cls->setConstructor<Propagate>();

}

AST_NAMESPACE_END