///
/// @file      VariableList.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-04-30
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
#include "AstScript/Variable.hpp"
#include <vector>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/


/// @brief 变量列表
/// @details 用于存储脚本中的变量
class AST_CORE_API VariableList
{
public:
    using ListType = std::vector<SharedPtr<Variable>>;
    using iterator = ListType::iterator;
    using const_iterator = ListType::const_iterator;


    VariableList() = default;
    ~VariableList() = default;
    void clear() { variables_.clear(); }
    void append(Variable* var) { variables_.push_back(var); }
    size_t size() const { return variables_.size(); }

    iterator begin() { return variables_.begin(); }
    iterator end() { return variables_.end(); }
    const_iterator begin() const { return variables_.begin(); }
    const_iterator end() const { return variables_.end(); }
    
private:
    ListType variables_;        ///< 变量列表
};



/*! @} */

AST_NAMESPACE_END
