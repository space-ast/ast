///
/// @file      RotationElement.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-02-20
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
#include <vector>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup SolarSystem
    @{
*/

class BKVParser;

/// @brief 行星旋转根数
/// @details 描述行星旋转状态的系数
class AST_CORE_API RotationElement
{
public:
    struct Coefficient
    {
        double amplitude_ {0.0};
        double constant_ {0.0};
        double rate_ {0.0};
        double rateDot_ {0.0};
    };
    
    RotationElement() = default;
    ~RotationElement() = default;

    double constant() const { return constant_; }
    double rate() const { return rate_; }
    double rateDot() const { return rateDot_; }
    bool isSine() const { return isSine_; }
    err_t load(BKVParser &parser);
protected:
    friend class RotationalData;

    /// @brief 计算多项式项和三角函数项
    double evaluate(double t_or_d,double t, double d) const;
    /// @brief 计算t=d=0时的多项式项和三角函数项
    double evaluateZero() const;

    /// @brief 仅计算多项式项
    double evaluatePoly(double t_or_d, double t) const;
    /// @brief 计算t=d=0时的多项式项
    double evaluatePolyZero() const;
protected:
    double constant_ {0.0};
    double rate_ {0.0};
    double rateDot_ {0.0};
    bool isSine_ {false};
    std::vector<Coefficient> coefficients_;
};


/*! @} */

AST_NAMESPACE_END
