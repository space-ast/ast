///
/// @file      ScStateCalcScript.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-05-12
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

#include "ScStateCalcScript.hpp"
#include "AstUtil/ScopedPtr.hpp"
#include "AstUtil/Logger.hpp"
#include "AstScript/ScriptExecutor.hpp"

AST_NAMESPACE_BEGIN

errc_t ScStateCalcScript::calculate(const SpacecraftState& scState, double& result)
{
    // 创建脚本执行器
    ScopedPtr<ScriptExecutor> executor(newScriptExecutor());
    if (executor == nullptr)
    {
        aError(_("创建脚本执行器失败"));
        return eErrorNullPtr;
    }
    // 设置变量值
    for(auto& var : variables_)
    {
        double value;
        errc_t rc = var->calculate(scState, value);
        if (rc != eNoError)
        {
            aError(_("计算变量 '%s' 失败"), var->getName().c_str());
            return rc;
        }
        rc = executor->setVariable(var->getName(), value);
        if (rc != eNoError)
        {
            aError(_("设置变量 '%s' 失败"), var->getName().c_str());
            return rc;
        }
    }
    // 对脚本表达式求值
    ScriptResult scriptResult;
    errc_t rc = executor->evaluate(expression_, &scriptResult);
    if (rc != eNoError || !scriptResult.value())
    {
        aError(_("表达式求值失败: '%s'"), expression_.c_str());
        return rc;
    }
    // 转换为double类型
    result = scriptResult.value()->toDouble();
    return eNoError;
}


AST_NAMESPACE_END
