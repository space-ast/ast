#include "DetectorMeanAnomaly.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class DetectorMeanAnomaly::staticType;

static bool DetectorMeanAnomaly_ClassInited = (DetectorMeanAnomaly::ClassInit(&DetectorMeanAnomaly::staticType), true);

void DetectorMeanAnomaly::ClassInit(Class* cls)
{

    cls->setName(N_("DetectorMeanAnomaly"));
    cls->addToRegistry();
    cls->setParent<EventDetector>();
    cls->setConstructor<DetectorMeanAnomaly>();

}

AST_NAMESPACE_END