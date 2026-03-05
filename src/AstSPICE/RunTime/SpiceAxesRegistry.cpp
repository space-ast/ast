///
/// @file      SpiceAxesRegister.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-05
/// @copyright 版权所有 (C) 2026-present, ast项目.
///
/// ast项目（https://github.com/space-ast/ast）
/// 本项目基于 Apache 2.0 开源许可证分发。
/// 您可在遵守许可证条款的前提下使用、修改和分发本软件。
/// 许可证全文请见：
/// 
///    http://www.apache.org/licenses/LICENSE-2.0
/// 
/// 重要须知：
/// 软件按"现有状态"提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。

#include "SpiceAxesRegistry.hpp"
#include "AstUtil/StringView.hpp"
#include "AstCore/AxesTransform.hpp"


AST_NAMESPACE_BEGIN

SpiceAxesRegistry &SpiceAxesRegistry::Instance()
{
    static SpiceAxesRegistry instance(true);
    return instance;
}

SpiceAxesRegistry::SpiceAxesRegistry(bool whetherInit)
{
    if(whetherInit)
    {
        init();
    }
}


PAxes SpiceAxesRegistry::findAxes(StringView name) const
{
    auto it = axesMap_.find(name.to_string());
    if (it == axesMap_.end())
    {
        return nullptr;
    }
    return it->second;
}

err_t SpiceAxesRegistry::init()
{
    axesMap_["ICRF"] = aAxesICRF();
    axesMap_["ECF"] = aAxesECF();
    axesMap_["J2000"] = aAxesJ2000();
    axesMap_["MOD"] = aAxesMOD();
    axesMap_["TOD"] = aAxesTOD();
    axesMap_["GTOD"] = aAxesGTOD();
    // axesMap_["B1950"] = aAxesB1950();
    axesMap_["B1950"] = aAxesB1950Spice();
    axesMap_["FK4"] = aAxesFK4();
    axesMap_["DE-118"] = aAxesDE118();
    axesMap_["DE-96"] = aAxesDE96();
    axesMap_["DE-102"] = aAxesDE102();
    axesMap_["DE-108"] = aAxesDE108();
    axesMap_["DE-111"] = aAxesDE111();
    axesMap_["DE-114"] = aAxesDE114();
    axesMap_["DE-122"] = aAxesDE122();
    axesMap_["DE-125"] = aAxesDE125();
    axesMap_["DE-130"] = aAxesDE130();
    axesMap_["Galactic"] = aAxesGalactic();
    axesMap_["DE-200"] = aAxesDE200();
    axesMap_["DE-202"] = aAxesDE202();
    axesMap_["MarsIAU"] = aAxesMarsIAU();
    axesMap_["EclipJ2000"] = aAxesEclipJ2000();
    axesMap_["EclipB1950"] = aAxesEclipB1950();
    axesMap_["DE-140"] = aAxesDE140();
    axesMap_["DE-142"] = aAxesDE142();
    axesMap_["DE-143"] = aAxesDE143();
    return eNoError;
}

AST_NAMESPACE_END

