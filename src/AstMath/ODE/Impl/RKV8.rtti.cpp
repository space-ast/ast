#include "RKV8.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class RKV8::staticType;

static bool RKV8_ClassInited = (RKV8::ClassInit(&RKV8::staticType), true);

void RKV8::ClassInit(Class* cls)
{

    cls->setName("RKV8");
    cls->setDesc(u8R"(RKV8 积分器)");
    cls->addToRegistry();
    cls->setParent<ODEFixedStepIntegrator>();
    cls->setConstructor<RKV8>();

}

AST_NAMESPACE_END