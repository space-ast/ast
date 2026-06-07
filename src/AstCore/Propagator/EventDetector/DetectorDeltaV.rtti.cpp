#include "DetectorDeltaV.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class DetectorDeltaV::staticType;

static bool DetectorDeltaV_ClassInited = (DetectorDeltaV::ClassInit(&DetectorDeltaV::staticType), true);

void DetectorDeltaV::ClassInit(Class* cls)
{

    cls->setName(N_("DetectorDeltaV"));
    cls->addToRegistry();
    cls->setParent<EventDetector>();
    cls->setConstructor<DetectorDeltaV>();

}

AST_NAMESPACE_END