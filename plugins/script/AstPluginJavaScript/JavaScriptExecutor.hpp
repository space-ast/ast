///
/// @file      JavaScriptExecutor.hpp
/// @brief     JavaScript 脚本执行器
/// @details   基于嵌入式 JavaScript 引擎（当前为 Duktape）执行 ECMAScript 脚本
/// @author    axel
/// @date      2026-10-27
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

#pragma once

#include "AstGlobal.h"
#include "AstScript/ScriptExecutor.hpp"
#include "AstPluginJavaScriptGlobal.hpp"
#include <memory>
#include <string>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup
    @{
*/

/// @brief JavaScript 脚本执行器
/// @details 每个实例拥有独立的脚本全局命名空间，实例之间变量互不影响
class AST_PLUGIN_JAVASCRIPT_API JavaScriptExecutor : public ScriptExecutor
{
    // 前向声明内部实现类
    class Impl;

public:
    using ScriptExecutor::execute;

    JavaScriptExecutor();

    JavaScriptExecutor(const JavaScriptExecutor&) = delete;
    JavaScriptExecutor& operator=(const JavaScriptExecutor&) = delete;

    ~JavaScriptExecutor() override;

    errc_t initialize() override;
    void finalize() override;
    errc_t execute(StringView script, ScriptResult* resultOut = nullptr) override;
    errc_t evaluate(StringView expression, ScriptResult* resultOut = nullptr) override;
    std::string getLastError() const override;

    // 下面这些重载会隐藏基类同名的其他重载（含字符串字面量重载），需要显式引回
    using ScriptExecutor::setVariable;

    errc_t setVariable(StringView name, StringView value) override;
    errc_t setVariable(StringView name, double value) override;
    errc_t setVariable(StringView name, int value) override;
    errc_t setVariable(StringView name, bool value) override;

    errc_t getVariable(StringView name, std::string& value) const override;
    errc_t getVariable(StringView name, double& value) const override;
    errc_t getVariable(StringView name, int& value) const override;
    errc_t getVariable(StringView name, bool& value) const override;

private:
    std::unique_ptr<Impl> impl_;
    mutable std::string lastError_{};
};

/*! @} */

AST_NAMESPACE_END
