#include "SegmentGraph.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class SegmentGraph::staticType;

static bool SegmentGraph_ClassInited = (SegmentGraph::ClassInit(&SegmentGraph::staticType), true);

void SegmentGraph::ClassInit(Class* cls)
{

    cls->setName("SegmentGraph");
    cls->addToRegistry();
    cls->setParent<Segment>();
    cls->setConstructor<SegmentGraph>();

}

AST_NAMESPACE_END