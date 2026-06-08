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
class VariableList
{
public:
    using ListType = std::vector<SharedPtr<Variable>>;
    using iterator = ListType::iterator;
    using const_iterator = ListType::const_iterator;


    VariableList() = default;
    ~VariableList() = default;
    void clear() { variables_.clear(); }
    void append(Variable* var) { variables_.push_back(var); }
    void erase(size_t index) { variables_.erase(variables_.begin() + index); }
    void move(size_t from, size_t to);
    size_t size() const { return variables_.size(); }

    Variable* at(size_t index) { return variables_.at(index).get(); }
    Variable* at(size_t index) const { return variables_.at(index).get(); }
    Variable* operator[](size_t index) { return variables_[index].get(); }
    Variable* operator[](size_t index) const { return variables_[index].get(); }

    iterator begin() { return variables_.begin(); }
    iterator end() { return variables_.end(); }
    const_iterator begin() const { return variables_.begin(); }
    const_iterator end() const { return variables_.end(); }

private:
    ListType variables_;        ///< 变量列表
};


inline void VariableList::move(size_t from, size_t to)
{
    if (from >= variables_.size() || to >= variables_.size() || from == to)
        return;
    SharedPtr<Variable> var = variables_[from];
    variables_.erase(variables_.begin() + from);
    size_t insertPos = (to > from) ? to - 1 : to;
    variables_.insert(variables_.begin() + insertPos, var);
}

/*! @} */

AST_NAMESPACE_END
