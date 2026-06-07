#include "DetectorEpoch.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class DetectorEpoch::staticType;

static bool DetectorEpoch_ClassInited = (DetectorEpoch::ClassInit(&DetectorEpoch::staticType), true);

void DetectorEpoch::ClassInit(Class* cls)
{

    cls->setName(N_("DetectorEpoch"));
    cls->addToRegistry();
    cls->setParent<EventDetector>();
    cls->setConstructor<DetectorEpoch>();

}

AST_NAMESPACE_END