///
/// @file      OpenVBSExecutor.hpp
/// @brief     基于 OpenVBS 的 VBScript 脚本执行器
/// @details   使用可移植的 OpenVBS（Jujube）脚本引擎执行 VBScript 语法的脚本，
///            不依赖 Windows Active Scripting，可在 Windows/Linux 上使用。
/// @author    axel
/// @date      2026-10-31
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
#include "AstPluginOpenVBSGlobal.hpp"
#include "AstScript/ScriptExecutor.hpp"
#include <memory>
#include <string>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup Script
    @{
*/

/// @brief 基于 OpenVBS 的 VBScript 脚本执行器
/// @warning 引擎本身不是线程安全的，同一个实例不可在多个线程中并发使用
class AST_PLUGIN_OPENVBS_API OpenVBSExecutor : public ScriptExecutor
{
    // 前向声明内部实现类
    class Impl;

public:
    using ScriptExecutor::execute;

    OpenVBSExecutor();

    OpenVBSExecutor(const OpenVBSExecutor&) = delete;
    OpenVBSExecutor& operator=(const OpenVBSExecutor&) = delete;

    ~OpenVBSExecutor() override;

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
};

/*! @} */

AST_NAMESPACE_END
