///
/// @file      OpBin.hpp
/// @brief     ~
/// @details   ~
/// @author    jinke18
/// @date      2025-12-19
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
#include "Op.hpp"
#include "AstScript/ScriptAPI.hpp"
#include <mutex>
#include <atomic>

AST_NAMESPACE_BEGIN

/// @brief 二元运算符表达式
class OpBin: public Op
{
public:
    AST_EXPR(OpBin)

public:
    OpBin(OpBinType op, Expr* left, Expr* right)
        : op_(op), left_(left), right_(right)
    {};
    virtual ~OpBin() = default;
    Value* eval() const override;
    std::string getExpression(Object* context=nullptr) const override;
protected:
    OpBinType   op_;        ///< 运算符
    Expr*       left_;      ///< 左操作数
    Expr*       right_;     ///< 右操作数

    mutable std::atomic<OpBinFunc> func_{nullptr};      ///< 运算符函数指针
    mutable std::atomic<Class*>    leftType_{nullptr};  ///< 左操作数类型
    mutable std::atomic<Class*>    rightType_{nullptr}; ///< 右操作数类型
    mutable std::mutex             cache_mutex_;        ///< 用于保护缓存成员的互斥锁
};



AST_NAMESPACE_END
