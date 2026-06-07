#include "ThirdBodyForce.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class ThirdBodyForce::staticType;

static bool ThirdBodyForce_ClassInited = (ThirdBodyForce::ClassInit(&ThirdBodyForce::staticType), true);

void ThirdBodyForce::ClassInit(Class* cls)
{

    cls->setName(N_("ThirdBodyForce"));
    cls->setDesc(u8R"(三体引力模型)");
    cls->addToRegistry();
    cls->setParent<ForceModel>();
    cls->setConstructor<ThirdBodyForce>();

}

AST_NAMESPACE_END