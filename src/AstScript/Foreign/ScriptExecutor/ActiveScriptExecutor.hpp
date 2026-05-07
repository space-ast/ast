///
/// @file      ActiveScript.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-04-29
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
#include "ScriptExecutor.hpp"
#include <memory>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

/// @brief 微软Active系列脚本执行器
/// @details 用于执行微软Active系列脚本: JScriptScript、JScript 等
///          通过COM接口 IActiveScript 等执行脚本
class AST_SCRIPT_API ActiveScriptExecutor: public ScriptExecutor
{
    // 前向声明内部实现类
    class Impl;

public:
    ActiveScriptExecutor();
    /// @brief 构造函数
    /// @details 允许指定脚本引擎 (如 L"JScript" / L"VBScript")
    /// @param progId 脚本引擎 ProgID
    explicit ActiveScriptExecutor(const wchar_t* progId); 
    ~ActiveScriptExecutor() override;

    errc_t initialize() override;
    void finalize() override;
    errc_t execute(StringView script, std::string* errorOut=nullptr) override;
    std::string getLastError() const override;

    errc_t setVariable(StringView name, StringView value) override;
    errc_t setVariable(StringView name, double value) override;
    errc_t setVariable(StringView name, int value) override;
    errc_t setVariable(StringView name, bool value) override;

    errc_t getVariable(StringView name, std::string& value) const override;
    errc_t getVariable(StringView name, double& value) const override;
    errc_t getVariable(StringView name, int& value) const override;
    errc_t getVariable(StringView name, bool& value) const override;

protected:
    /// 设置脚本引擎 ProgID（通常由子类如 JScriptExecutor 在构造时调用）
    void setProgID(const wchar_t* progId);

protected:
    std::unique_ptr<Impl> impl_; // 隐藏 COM 对象与状态
};

/*! @} */

AST_NAMESPACE_END