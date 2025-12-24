///
/// @file      SymbolScope.hpp
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

#pragma once

#include "AstGlobal.h"
#include <unordered_map>
#include <string>
#include <vector>
#include "AstUtil/SharedPtr.hpp"
#include "AstScript/Expr.hpp"

AST_NAMESPACE_BEGIN



/// @brief     符号作用域
/// @details   ~
class SymbolScope{
public:
    using SymbolMap = std::unordered_map<std::string, SharedPtr<Expr>>;
    using SymbolPair = std::pair<std::string, SharedPtr<Expr>>;
    using SymbolList = std::vector<SymbolPair>;

    /// @brief 默认构造函数
    SymbolScope() = default;

    /// @brief 构造函数，指定父符号作用域
    /// @param parent 父符号作用域指针
    explicit SymbolScope(SymbolScope* parent)
        : parent_(parent)
    {}

    ~SymbolScope() = default;

    /// @brief 添加符号到符号作用域
    /// @param name 符号名称
    /// @param expr 符号对应的表达式
    /// @return 是否添加成功（如果符号已存在则返回false）
    bool addSymbol(StringView name, Expr* expr);

    /// @brief 添加或更新符号
    /// @param name 符号名称
    /// @param expr 符号对应的表达式
    void setSymbol(StringView name, Expr* expr);

    /// @brief 查找符号作用域中的符号
    /// @param name 符号名称
    /// @param searchParent 是否向上查找父符号作用域
    /// @return 符号对应的表达式，如果不存在则返回nullptr
    Expr* findSymbol(StringView name, bool searchParent = true) const;

    /// @brief 解析符号作用域中的符号，如果不存在则创建一个新的变量
    /// @param name 符号名称
    /// @return 符号对应的表达式指针
    Expr* resolveSymbol(StringView name);

    /// @brief 删除符号
    /// @param name 符号名称
    /// @return 是否删除成功
    bool removeSymbol(StringView name);

    /// @brief 检查符号是否存在
    /// @param name 符号名称
    /// @param searchParent 是否向上查找父符号表
    /// @return 符号是否存在
    bool hasSymbol(StringView name, bool searchParent = true) const;

    /// @brief 获取当前符号表中的所有符号
    /// @return 符号列表
    SymbolList getSymbols() const;

    /// @brief 获取父符号作用域
    /// @return 父符号作用域指针
    SymbolScope* parent() const;

    /// @brief 设置父符号作用域
    /// @param parent 父符号作用域指针
    void setParent(SymbolScope* parent);

protected:
    SymbolMap    symbols_;              ///< 符号作用域中的符号
    SymbolScope* parent_ = nullptr;     ///< 父符号作用域指针
};

AST_NAMESPACE_END