///
/// @file      ODEVectorCollector.hpp
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
#include "AstMath/ODEStateObserver.hpp"
#include <vector>

AST_NAMESPACE_BEGIN

/// @brief ODE积分的状态向量收集器
/// @details 
/// 该类用于收集ODE积分过程中的状态向量。
class AST_MATH_API ODEStateVectorCollector: public ODEStateObserver
{
public:
    ODEStateVectorCollector(int ndim);
    ~ODEStateVectorCollector() = default;

    EODEAction onStateUpdate(double* y, double& x, ODEIntegrator* integrator) override;

    std::vector<double>& x(){ return x_; };
    std::vector<std::vector<double>>& y(){ return y_; };
protected:
    int ndim_;
    std::vector<double> x_;
    std::vector<std::vector<double>> y_;
};

AST_NAMESPACE_END
