#include "DifferentialCorrectorProfile.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class DifferentialCorrectorProfile::staticType;

static bool DifferentialCorrectorProfile_ClassInited = (DifferentialCorrectorProfile::ClassInit(&DifferentialCorrectorProfile::staticType), true);

void DifferentialCorrectorProfile::ClassInit(Class* cls)
{

    cls->setName("DifferentialCorrectorProfile");
    cls->setDesc(u8R"(微分修正器配置，用于求解非线性方程，可以用于修正轨道参数以满足约束条件要求)");
    cls->addToRegistry();
    cls->setParent<TargeterProfile>();
    cls->setConstructor<DifferentialCorrectorProfile>();

    cls->addProperty("useHomotopy", aNewPropertyBool<DifferentialCorrectorProfile, &DifferentialCorrectorProfile::useHomotopy, &DifferentialCorrectorProfile::setUseHomotopy>());
    cls->addProperty("numHomotopySteps", aNewPropertyInt<DifferentialCorrectorProfile, &DifferentialCorrectorProfile::numHomotopySteps, &DifferentialCorrectorProfile::setNumHomotopySteps>());
    cls->addProperty("useLineSearch", aNewPropertyBool<DifferentialCorrectorProfile, &DifferentialCorrectorProfile::useLineSearch, &DifferentialCorrectorProfile::setUseLineSearch>());
    cls->addProperty("lineSearchTolerance", aNewPropertyDouble<DifferentialCorrectorProfile, &DifferentialCorrectorProfile::lineSearchTolerance, &DifferentialCorrectorProfile::setLineSearchTolerance>());
    cls->addProperty("lineSearchMaxNumCalls", aNewPropertyInt<DifferentialCorrectorProfile, &DifferentialCorrectorProfile::lineSearchMaxNumCalls, &DifferentialCorrectorProfile::setLineSearchMaxNumCalls>());
    cls->addProperty("lineSearchLowerBound", aNewPropertyDouble<DifferentialCorrectorProfile, &DifferentialCorrectorProfile::lineSearchLowerBound, &DifferentialCorrectorProfile::setLineSearchLowerBound>());
    cls->addProperty("lineSearchUpperBound", aNewPropertyDouble<DifferentialCorrectorProfile, &DifferentialCorrectorProfile::lineSearchUpperBound, &DifferentialCorrectorProfile::setLineSearchUpperBound>());
    cls->addProperty("maxIterations", aNewPropertyInt<DifferentialCorrectorProfile, &DifferentialCorrectorProfile::maxIterations, &DifferentialCorrectorProfile::setMaxIterations>());
}

AST_NAMESPACE_END