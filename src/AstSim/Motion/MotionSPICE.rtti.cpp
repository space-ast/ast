#include "MotionSPICE.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class MotionSPICE::staticType;

static bool MotionSPICE_ClassInited = (MotionSPICE::ClassInit(&MotionSPICE::staticType), true);

void MotionSPICE::ClassInit(Class* cls)
{

    cls->setName(NC_("Class", "MotionSPICE"));
    cls->setDesc(u8R"(SPICE星历预报器/运动模型类)");
    cls->addToRegistry();
    cls->setParent<MotionWithIntervalStep>();
    cls->setConstructor<MotionSPICE>();

}

AST_NAMESPACE_END