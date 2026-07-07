///
/// @file      StudyWorkbench.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-06-03
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
#include "AstCore/Command.hpp"
#include "AstCore/VariableList.hpp"
#include "AstScript/Interpreter.hpp"
#include <memory>


AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/


/// @brief  研究工作台，用于参数扫描研究、优化研究、不确定性分析、区间分析等分析
class AST_ANALYZER_API StudyWorkbench : public Command
{
public:
    AST_OBJECT(StudyWorkbench)
public:
    StudyWorkbench();

    errc_t execute() override;
    
    /// @brief 获取输入变量列表
    VariableList& inputs(){return inputs_;}
    const VariableList& inputs() const {return inputs_;}

    /// @brief 获取输出变量列表
    VariableList& outputs(){return outputs_;}
    const VariableList& outputs() const {return outputs_;}

    /// @brief 获取关联的执行命令
    Command* relatedCommand() const {return relatedCommand_.get();}

    /// @brief 设置关联的执行命令
    void setRelatedCommand(Command* command) { relatedCommand_ = command; }

    /// @brief 获取脚本解释器
    Interpreter* interpreter() const {return interpreter_.get();}
private:
    VariableList inputs_;                               ///< 输入变量列表
    VariableList outputs_;                              ///< 输出变量列表
    mutable WeakPtr<Command> relatedCommand_;           ///< 所关联的执行命令
private:
    mutable std::unique_ptr<Interpreter> interpreter_;  ///< 解释器
};



/*! @} */

AST_NAMESPACE_END

