///
/// @file      ScriptingTool.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-04-28
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
#include "TargeterProfile.hpp"
#include "AstScript/ScriptExecutor.hpp"
#include "AstScript/Variable.hpp"
#include "AstUtil/StringView.hpp"
#include "AstCore/VariableList.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/



/// @brief 脚本工具配置
class AST_CORE_API ScriptingToolProfile : public TargeterProfile
{
public:
    AST_OBJECT(ScriptingToolProfile)

    static ScriptingToolProfile* New();

    /// @brief 执行脚本工具配置
    /// @return errc_t 执行结果
    errc_t execute() override;

    ScriptingToolProfile() = default;
    ~ScriptingToolProfile() override = default;

public:
    EScriptLanguage language() const { return language_; }
    void setLanguage(EScriptLanguage language) { language_ = language; }

    const std::string& scriptStatements() const { return scriptStatements_; }
    void setScriptStatements(StringView scriptStatements) { scriptStatements_ = std::string(scriptStatements); }

    const VariableList& attributes() const { return attributes_; }
    VariableList& attributes() { return attributes_; }

    const VariableList& parameters() const { return parameters_; }
    VariableList& parameters() { return parameters_; }

    const VariableList& calcObjects() const { return calcObjects_; }
    VariableList& calcObjects() { return calcObjects_; }

    /// @brief 获取参数变量
    Variable* getParameter(StringView name) const;

private:
    EScriptLanguage language_{EScriptLanguage::ePython};    ///< 脚本语言
    std::string scriptStatements_;                          ///< 脚本语句
    VariableList attributes_;                               ///< 属性变量列表
    VariableList parameters_;                               ///< 参数变量列表
    VariableList calcObjects_;                              ///< 计算对象变量列表
};

/*! @} */

AST_NAMESPACE_END


