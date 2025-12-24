///
/// @file      SymbolScope.cpp
/// @brief     ~
/// @details   ~
/// @author    jinke18
/// @date      2025-12-23
/// @copyright 版权所有 (C) 2025-present, ast项目.
///
/// ast项目（https://github.com/space-ast/ast）
/// 本项目基于 Apache 2.0 开源许可证分发。
/// 您可在遵守许可证条款的前提下使用、修改和分发本软件。
/// 许可证全文请见：
/// 
///    http://www.apache.org/licenses/LICENSE-2.0
/// 
/// 重要须知：
/// 软件按"现有状态"提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。

#include "SymbolScope.hpp"
#include "AstScript/Variable.hpp"

AST_NAMESPACE_BEGIN

bool SymbolScope::addSymbol(StringView name, Expr* expr)
{
    // 如果符号已存在，则返回false
    if (symbols_.find(name.to_string()) != symbols_.end()) {
        return false;
    }

    symbols_[name.to_string()] = expr;
    return true;
}

void SymbolScope::setSymbol(StringView name, Expr* expr)
{
    symbols_[name.to_string()] = expr;
}

Expr* SymbolScope::findSymbol(StringView name, bool searchParent) const
{
    // 首先在当前符号表查找
    auto it = symbols_.find(name.to_string());
    if (it != symbols_.end()) {
        return it->second.get();
    }

    // 如果允许查找父符号表且父符号表存在，则递归查找
    if (searchParent && parent_) {
        return parent_->findSymbol(name, searchParent);
    }

    // 找不到符号，返回nullptr
    return nullptr;
}

Expr *SymbolScope::resolveSymbol(StringView name) 
{
    // 首先在当前符号表查找
    auto name_ = name.to_string();
    auto it = symbols_.find(name_);
    if (it != symbols_.end()) {
        return it->second.get();
    }
    // 如果不存在，则创建一个新的变量
    auto var = new Variable(name_);
    symbols_[name_] = var;
    return var;
}

bool SymbolScope::removeSymbol(StringView name)
{
    auto it = symbols_.find(name.to_string());
    if (it != symbols_.end()) {
        symbols_.erase(it);
        return true;
    }

    return false;
}

bool SymbolScope::hasSymbol(StringView name, bool searchParent) const
{
    // 首先在当前符号表查找
    if (symbols_.find(name.to_string()) != symbols_.end()) {
        return true;
    }

    // 如果允许查找父符号表且父符号表存在，则递归查找
    if (searchParent && parent_) {
        return parent_->hasSymbol(name, searchParent);
    }

    // 找不到符号
    return false;
}

SymbolScope::SymbolList SymbolScope::getSymbols() const
{
    SymbolList result;
    result.reserve(symbols_.size());

    for (const auto& pair : symbols_) {
        result.emplace_back(pair.first, pair.second);
    }

    return result;
}

SymbolScope* SymbolScope::parent() const
{
    return parent_;
}

void SymbolScope::setParent(SymbolScope* parent)
{
    parent_ = parent;
}

AST_NAMESPACE_END