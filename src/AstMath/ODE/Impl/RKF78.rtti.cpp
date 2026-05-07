#include "RKF78.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class RKF78::staticType;

static bool RKF78_ClassInited = (RKF78::ClassInit(&RKF78::staticType), true);

void RKF78::ClassInit(Class* cls)
{

    cls->setName("RKF78");
    cls->setDesc(u8R"(RKF78 积分器)");
    cls->addToRegistry();
    cls->setParent<ODEVarStepIntegrator>();
    cls->setConstructor<RKF78>();

}

AST_NAMESPACE_END