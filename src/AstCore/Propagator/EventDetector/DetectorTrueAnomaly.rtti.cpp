#include "DetectorTrueAnomaly.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class DetectorTrueAnomaly::staticType;

static bool DetectorTrueAnomaly_ClassInited = (DetectorTrueAnomaly::ClassInit(&DetectorTrueAnomaly::staticType), true);

void DetectorTrueAnomaly::ClassInit(Class* cls)
{

    cls->setName(NC_("Class", "DetectorTrueAnomaly"));
    cls->addToRegistry();
    cls->setParent<EventDetector>();
    cls->setConstructor<DetectorTrueAnomaly>();

}

AST_NAMESPACE_END