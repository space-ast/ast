///
/// @file      ODEVarStepIntegrator.hpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2026-01-16
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
#include "ODEFixedStepIntegrator.hpp"


AST_NAMESPACE_BEGIN

/// @brief 可变步长积分器
/// @details 可变步长积分器每步积分的步长自适应调整，以达到目标误差。
/// @ingroup ODE
class AST_MATH_API ODEVarStepIntegrator : public ODEFixedStepIntegrator
{
public:
    ODEVarStepIntegrator();
    ~ODEVarStepIntegrator();
    using ODEFixedStepIntegrator::integrate;

    /// @see ODEIntegrator
    err_t integrate(ODE& ode, double* y, double& t, double tf) final;

    /// @see ODEIntegrator
    err_t integrateStep(ODE& ode, double* y, double& t, double tf) final;

    /// @brief 设置最大绝对误差
    void setMaxAbsErr(double maxAbsErr){maxAbsErr_ = maxAbsErr;};
    /// @brief 设置最大相对误差
    void setMaxRelErr(double maxRelErr){maxRelErr_ = maxRelErr;};

    /// @brief 设置初始步长
    void setInitialStepSize(double initialStepSize){setStepSize(initialStepSize);};

    /// @brief 获取积分过程中统计到的最大步长
    double getLargestStepSize() const;

    /// @brief 获取积分过程中统计到的最小步长
    double getSmallestStepSize() const;

    using ODEFixedStepIntegrator::getNumSteps;
protected:
    /// @brief 判断是否满足误差要求
    /// @param absh 绝对步长
    /// @param y 当前状态向量
    /// @param ynew 新状态向量
    /// @return 是否满足误差要求
    bool isErrorMeet(double& absh, const double* y, const double* ynew);
private:
    bool useMinStep_;           ///< 是否使用最小步长
    bool useMaxStep_;           ///< 是否使用最大步长
    // bool useFixedStepSize_;     ///< 是否使用固定步长
    bool warnOnMinStep_;        ///< 是否警告最小步长
    int  maxStepAttempts_;      ///< 自适应步长尝试次数
    double minStepSize_;        ///< 最小步长
    double maxStepSize_;        ///< 最大步长
    double maxAbsErr_;          ///< 最大绝对误差
    double maxRelErr_;          ///< 最大相对误差
    double minStepScaleFactor_; ///< 最小步长缩放因子
    double maxStepScaleFactor_; ///< 最大步长缩放因子
    double safetyCoeffLow_;     ///< 低安全系数
    double safetyCoeffHigh_;    ///< 高安全系数
    double errCtrPowthLow_;     ///< 低误差控制指数
    double errCtrPowthHigh_;    ///< 高误差控制指数
};

AST_NAMESPACE_END
