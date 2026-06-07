#include "DetectorLighting.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class DetectorLighting::staticType;

static bool DetectorLighting_ClassInited = (DetectorLighting::ClassInit(&DetectorLighting::staticType), true);

void DetectorLighting::ClassInit(Class* cls)
{

    cls->setName(N_("DetectorLighting"));
    cls->addToRegistry();
    cls->setParent<EventDetector>();
    cls->setConstructor<DetectorLighting>();

}

AST_NAMESPACE_END