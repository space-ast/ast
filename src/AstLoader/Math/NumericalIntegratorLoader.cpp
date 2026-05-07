///
/// @file      NumericalIntegratorLoader.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-04-21
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
/// 本软件基于 Apache 2.0 开源许可证分发。
/// 您可在遵守许可证条款的前提下使用、修改和分发本软件。
/// 许可证全文请见：
/// 
///    http://www.apache.org/licenses/LICENSE-2.0
/// 
/// 重要须知：
/// 软件按"现有状态"提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。

#include "NumericalIntegratorLoader.hpp"
#include "AstScript/Value.hpp"
#include "AstMath/RKF78.hpp"
#include "AstMath/RKF56.hpp"
#include "AstMath/RKF45.hpp"
#include "AstMath/RK8.hpp"
#include "AstUtil/Logger.hpp"
#include <string>

AST_NAMESPACE_BEGIN

errc_t aLoadNumericalIntegrator(const Value &value, ScopedPtr<ODEIntegrator> &integrator)
{
    ScopedPtr<ODEVarStepIntegrator> varStepIntegrator;
    std::string type = value["Type"];
    if(type == "RKF7th8th")
    {
        varStepIntegrator = new RKF78();
    }
    else if(type == "RK4th5th")
    {
        varStepIntegrator = new RKF45();
    }
    else
    {
        aError("unsupported numerical integrator type: '%s', use RKF78 by default", type.c_str());
        varStepIntegrator = new RKF78();
    }
    varStepIntegrator->setInitialStepSize(value["InitialStep"]);
    varStepIntegrator->setMaxAbsErr(value["MaxAbsError"]);
    varStepIntegrator->setMaxIterations(value["MaxIterations"]);
    varStepIntegrator->setMaxRelErr(value["MaxRelError"]);
    varStepIntegrator->setMaxStepSize(value["MaxStep"]);
    varStepIntegrator->setMinStepSize(value["MinStep"]);
    varStepIntegrator->setSafetyCoeffHigh(value["SCHigh"]);
    varStepIntegrator->setSafetyCoeffLow(value["SCLow"]);
    varStepIntegrator->setUseMinStep(value["UseMinStep"]);
    varStepIntegrator->setUseMaxStep(value["UseMaxStep"]);
    // varStepIntegrator->setUseFixedStep(value["UseFixedStep"]);

    integrator = varStepIntegrator.release();
    return eNoError;
}

AST_NAMESPACE_END
