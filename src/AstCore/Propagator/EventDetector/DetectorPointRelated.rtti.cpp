#include "DetectorPointRelated.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class DetectorPointRelated::staticType;

static bool DetectorPointRelated_ClassInited = (DetectorPointRelated::ClassInit(&DetectorPointRelated::staticType), true);

void DetectorPointRelated::ClassInit(Class* cls)
{

    cls->setName("DetectorPointRelated");
    cls->addToRegistry();
    cls->setParent<EventDetector>();
    cls->setConstructor<DetectorPointRelated>();

}

AST_NAMESPACE_END