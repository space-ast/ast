#include "Sequence.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class Sequence::staticType;

static bool Sequence_ClassInited = (Sequence::ClassInit(&Sequence::staticType), true);

void Sequence::ClassInit(Class* cls)
{

    cls->setName("Sequence");
    cls->setDesc(u8R"(任务序列段，内部包含多个任务命令或轨道段，并会按顺序执行)");
    cls->addToRegistry();
    cls->setParent<Segment>();
    cls->setConstructor<Sequence>();

}

AST_NAMESPACE_END