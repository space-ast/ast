#include "RKF45.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class RKF45::staticType;

static bool RKF45_ClassInited = (RKF45::ClassInit(&RKF45::staticType), true);

void RKF45::ClassInit(Class* cls)
{

    cls->setName("RKF45");
    cls->setDesc(u8R"(Runge-Kutta-Fehlberg 4(5) 积分器)");
    cls->addToRegistry();
    cls->setParent<ODEVarStepIntegrator>();
    cls->setConstructor<RKF45>();

}

AST_NAMESPACE_END