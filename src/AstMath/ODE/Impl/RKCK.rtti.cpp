#include "RKCK.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class RKCK::staticType;

static bool RKCK_ClassInited = (RKCK::ClassInit(&RKCK::staticType), true);

void RKCK::ClassInit(Class* cls)
{

    cls->setName("RKCK");
    cls->setDesc(u8R"(Runge-Kutta Cash-Karp 方法)");
    cls->addToRegistry();
    cls->setParent<ODEVarStepIntegrator>();
    cls->setConstructor<RKCK>();

}

AST_NAMESPACE_END