#include "SegmentGraph.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class SegmentGraph::staticType;

static bool SegmentGraph_ClassInited = (SegmentGraph::ClassInit(&SegmentGraph::staticType), true);

void SegmentGraph::ClassInit(Class* cls)
{

    cls->setName(N_("SegmentGraph"));
    cls->setDesc(u8R"(轨道段框图，用类似simulink的框图描述复杂、多航天的航天任务中的轨道任务段)");
    cls->addToRegistry();
    cls->setParent<Segment>();
    cls->setConstructor<SegmentGraph>();

}

AST_NAMESPACE_END