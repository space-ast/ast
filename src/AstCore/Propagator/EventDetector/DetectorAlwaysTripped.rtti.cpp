#include "DetectorAlwaysTripped.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class DetectorAlwaysTripped::staticType;

static bool DetectorAlwaysTripped_ClassInited = (DetectorAlwaysTripped::ClassInit(&DetectorAlwaysTripped::staticType), true);

void DetectorAlwaysTripped::ClassInit(Class* cls)
{

    cls->setName(NC_("Class", "DetectorAlwaysTripped"));
    cls->addToRegistry();
    cls->setParent<EventDetector>();
    cls->setConstructor<DetectorAlwaysTripped>();

}

AST_NAMESPACE_END