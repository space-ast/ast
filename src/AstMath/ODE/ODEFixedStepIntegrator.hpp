///
/// @file      ODEFixedStepIntegrator.hpp
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
#include "ODEIntegrator.hpp"
#include "ODEWorkspace.hpp"

AST_NAMESPACE_BEGIN


/// @brief 固定步长积分器
/// @details 固定步长积分器是一种简单的积分器，每步积分的时间间隔相同。
class AST_MATH_API ODEFixedStepIntegrator : public ODEIntegrator
{
public:
    class Workspace;
    ODEFixedStepIntegrator();
    ~ODEFixedStepIntegrator();
    using ODEIntegrator::integrate;

    /// @brief 获取步长
    double getStepSize() const{ return stepSize_; }
    
    /// @brief 设置步长
    void setStepSize(double stepSize){ stepSize_ = stepSize; }

    /// @brief 获取积分过程中统计到的积分步数
    int getNumSteps() const;

    /// @brief 获取工作空间
    const Workspace& getWorkspace() const{ return workspace_; }
    Workspace& getWorkspace(){ return workspace_; }

    /// @see ODEIntegrator
    err_t integrate(ODE& ode, double* y, double& t, double tf) override;

    /// @see ODEIntegrator
    err_t integrateStep(ODE& ode, double* y, double& t, double tf) override;

public:
    /// @brief 固定步长积分器工作空间
    class Workspace : public ODEWorkspace
    {
    public:
        Workspace();
        ~Workspace();
        void reset(int dimension, int stage);
        void clear();
    public:
        int numSteps_;              ///< 总步数(统计数据)
        int dimension_;             ///< 状态向量维度
        int stage_;                 ///< 多步法阶数
        double largestStepSize_;    ///< 最大步长(统计数据)
        double smallestStepSize_;   ///< 最小步长(统计数据)
        double** KArr_;             ///< 多步法中间结果数组
        double* absErrPerLen_;      ///< 每步绝对误差数组
        double* ymid_;              ///< 每步中点状态向量
        double* y_;                 ///< 每步状态向量
        double* ynew_;              ///< 每步新状态向量
        double* ystep_;             ///< 每步状态向量增量
        double  nextAbsStepSize_;   ///< 下一步绝对步长，用于步长自适应调整
    };
private:
    Workspace workspace_;           ///< 工作空间
    double stepSize_;               ///< 步长
};

AST_NAMESPACE_END
