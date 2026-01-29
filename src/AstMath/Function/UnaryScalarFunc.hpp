///
/// @file      UnaryScalarFunc.hpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2026-01-25
/// @copyright 版权所有 (C) 2026-present, ast项目.
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
#include "UnaryVectorFunc.hpp"

AST_NAMESPACE_BEGIN


/// @brief 一元标量值函数
class UnaryScalarFunc: public UnaryVectorFunc
{
public:
    virtual ~UnaryScalarFunc() = default;

    /// @brief 计算标量值
    /// @param x 输入值
    /// @param out 输出标量
    virtual void evaluate(double x, double* out) const override = 0;

    /// @brief 获取方程维度
    /// @return 方程维度
    int getDimension() const final{ return 1; }
};


/// @brief 泛型一元函数适配器
template<typename Func>
class UnaryScalarGenericFunc : public UnaryScalarFunc {
public:
    explicit UnaryScalarGenericFunc(Func func) 
        : func_(std::move(func)) 
    {}
    
    void evaluate(double x, double* out) const override {
        *out = func_(x);
    }
    
private:
    Func func_;
};

AST_NAMESPACE_END
