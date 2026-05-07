///
/// @file      PropagatorLoader.cpp
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

#include "PropagatorLoader.hpp"
#include "ForceModelLoader.hpp"
#include "AstCore/HPOP.hpp"
#include "AstUtil/StringView.hpp"
#include "AstUtil/Logger.hpp"
#include "AstLoader/ValXMLLoader.hpp"
#include "AstLoader/NumericalIntegratorLoader.hpp"
#include "AstCore/HPOPForceModel.hpp"

AST_NAMESPACE_BEGIN


errc_t aLoadPropagator(StringView filename, HPOP& propagator)
{
    SharedPtr<Value> value;
    errc_t rc = aLoadValue(filename, value);
    if(rc || !value)
        return rc;
    return aLoadPropagator(*value, propagator);
}

errc_t aLoadPropagator(const Value& value, HPOP& propagator)
{
    if(value["Type"].toString() != "NumericalPropagatorWrapper")
    {
        aError("invalid type, expect 'NumericalPropagatorWrapper'");
        return eErrorInvalidParam;
    }
    // 加载数值积分器
    ScopedPtr<ODEIntegrator> integrator;
    errc_t rc = aLoadNumericalIntegrator(value["NumericalIntegrator"], integrator);
    if(rc)
    {
        aWarning("failed to load numerical integrator");
    }
    propagator.setIntegrator(integrator.release()); // 转移所有权
    // 加载力模型
    HPOPForceModel forceModel{};
    rc = aLoadForceModel(value, forceModel);
    if(rc)
    {
        aWarning("failed to load force model");
    }
    propagator.setForceModel(forceModel);
    return eNoError;
}



AST_NAMESPACE_END


