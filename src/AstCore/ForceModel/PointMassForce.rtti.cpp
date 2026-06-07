#include "PointMassForce.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class PointMassForce::staticType;

static bool PointMassForce_ClassInited = (PointMassForce::ClassInit(&PointMassForce::staticType), true);

void PointMassForce::ClassInit(Class* cls)
{

    cls->setName(N_("PointMassForce"));
    cls->setDesc(u8R"(点质量引力模型/)");
    cls->addToRegistry();
    cls->setParent<BodyAttraction>();
    cls->setConstructor<PointMassForce>();

}

AST_NAMESPACE_END