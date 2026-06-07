#include "DetectorCartesian.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class DetectorCartesian::staticType;

static bool DetectorCartesian_ClassInited = (DetectorCartesian::ClassInit(&DetectorCartesian::staticType), true);

void DetectorCartesian::ClassInit(Class* cls)
{

    cls->setName(N_("DetectorCartesian"));
    cls->addToRegistry();
    cls->setParent<EventDetector>();
    cls->setConstructor<DetectorCartesian>();

}

AST_NAMESPACE_END