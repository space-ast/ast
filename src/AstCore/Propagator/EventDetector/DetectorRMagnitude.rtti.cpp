#include "DetectorRMagnitude.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class DetectorRMagnitude::staticType;

static bool DetectorRMagnitude_ClassInited = (DetectorRMagnitude::ClassInit(&DetectorRMagnitude::staticType), true);

void DetectorRMagnitude::ClassInit(Class* cls)
{

    cls->setName("DetectorRMagnitude");
    cls->addToRegistry();
    cls->setParent<DetectorPointRelated>();
    cls->setConstructor<DetectorRMagnitude>();

}

AST_NAMESPACE_END