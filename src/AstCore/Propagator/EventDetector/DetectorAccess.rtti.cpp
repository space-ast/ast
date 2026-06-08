#include "DetectorAccess.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class DetectorAccess::staticType;

static bool DetectorAccess_ClassInited = (DetectorAccess::ClassInit(&DetectorAccess::staticType), true);

void DetectorAccess::ClassInit(Class* cls)
{

    cls->setName(NC_("Class", "DetectorAccess"));
    cls->addToRegistry();
    cls->setParent<EventDetector>();
    cls->setConstructor<DetectorAccess>();

}

AST_NAMESPACE_END