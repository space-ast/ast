#include "ODEVarStepIntegrator.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class ODEVarStepIntegrator::staticType;

static bool ODEVarStepIntegrator_ClassInited = (ODEVarStepIntegrator::ClassInit(&ODEVarStepIntegrator::staticType), true);

void ODEVarStepIntegrator::ClassInit(Class* cls)
{

    cls->setName("ODEVarStepIntegrator");
    cls->setDesc(u8R"(可变步长积分器)");
    cls->addToRegistry();
    cls->setParent<ODEFixedStepIntegrator>();
    cls->setConstructor<ODEVarStepIntegrator>();

    cls->addProperty("useMinStep", aNewPropertyBoolMem<ODEVarStepIntegrator, &ODEVarStepIntegrator::useMinStep_>());
    cls->addProperty("useMaxStep", aNewPropertyBoolMem<ODEVarStepIntegrator, &ODEVarStepIntegrator::useMaxStep_>());
    cls->addProperty("warnOnMinStep", aNewPropertyBoolMem<ODEVarStepIntegrator, &ODEVarStepIntegrator::warnOnMinStep_>());
    cls->addProperty("maxStepAttempts", aNewPropertyIntMem<ODEVarStepIntegrator, &ODEVarStepIntegrator::maxStepAttempts_>());
    cls->addProperty("minStepSize", aNewPropertyDoubleMem<ODEVarStepIntegrator, &ODEVarStepIntegrator::minStepSize_>());
    cls->addProperty("maxStepSize", aNewPropertyDoubleMem<ODEVarStepIntegrator, &ODEVarStepIntegrator::maxStepSize_>());
    cls->addProperty("maxAbsErr", aNewPropertyDoubleMem<ODEVarStepIntegrator, &ODEVarStepIntegrator::maxAbsErr_>());
    cls->addProperty("maxRelErr", aNewPropertyDoubleMem<ODEVarStepIntegrator, &ODEVarStepIntegrator::maxRelErr_>());
    cls->addProperty("minStepScaleFactor", aNewPropertyDoubleMem<ODEVarStepIntegrator, &ODEVarStepIntegrator::minStepScaleFactor_>());
    cls->addProperty("maxStepScaleFactor", aNewPropertyDoubleMem<ODEVarStepIntegrator, &ODEVarStepIntegrator::maxStepScaleFactor_>());
    cls->addProperty("safetyCoeffLow", aNewPropertyDoubleMem<ODEVarStepIntegrator, &ODEVarStepIntegrator::safetyCoeffLow_>());
    cls->addProperty("safetyCoeffHigh", aNewPropertyDoubleMem<ODEVarStepIntegrator, &ODEVarStepIntegrator::safetyCoeffHigh_>());
    cls->addProperty("errCtrPowthLow", aNewPropertyDoubleMem<ODEVarStepIntegrator, &ODEVarStepIntegrator::errCtrPowthLow_>());
    cls->addProperty("errCtrPowthHigh", aNewPropertyDoubleMem<ODEVarStepIntegrator, &ODEVarStepIntegrator::errCtrPowthHigh_>());
}

AST_NAMESPACE_END