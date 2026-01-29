///
/// @file      ODEEventDetector.hpp
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

AST_NAMESPACE_BEGIN

/// @brief ODE积分的事件检测器
/// @details 
/// 该类用于检测ODE积分过程中的事件，例如根查找、事件触发等。
/// 参考matlab的ODE45等积分器的Events选项，以及hipparchus的ODEEventDetector类。
class AST_MATH_API ODEEventDetector
{
public:
    /// @brief 事件检测开关函数的条件
    enum EDirection
    {
        eDecrease = -1,     ///< 事件检测开关函数值减小时触发事件
        eBoth = 0,          ///< 事件检测开关函数值增大或减小时触发事件
        eIncrease = 1,      ///< 事件检测开关函数值增大时触发事件
    };

    ODEEventDetector() = default;
    virtual ~ODEEventDetector() = default;

    /// @brief 用于事件检测的开关函数与目标值的差值
    /// @param x 当前积分变量
    /// @param y 当前状态向量
    /// @return 用于事件检测的开关函数值与目标值的差值
    virtual double getDifference(const double* y,double x) const{return getValue(y, x) - goal_;}

    /// @brief 用于事件检测的开关函数值
    /// @param y 当前状态向量
    /// @param x 当前积分变量
    /// @return 用于事件检测的开关函数值
    virtual double getValue(const double* y,double x) const = 0;

    /// @brief 事件触发后的重复次数
    /// @return 事件触发后的重复次数
    int getRepeatCount() const { return repeatCount_; }
    void setRepeatCount(int repeatCount) { repeatCount_ = repeatCount; }

    /// @brief 事件检测开关函数的方向
    /// @return 事件检测开关函数的方向
    EDirection getDirection() const { return direction_; }
    void setDirection(EDirection direction) { direction_ = direction; }

    /// @brief 事件检测开关函数的阈值
    /// @return 事件检测开关函数的阈值
    double getThreshold() const { return threshold_; }
    void setThreshold(double threshold) { threshold_ = threshold; }

    /// @brief 事件检测的目标值
    /// @return 事件检测的目标值
    double getGoal() const { return goal_; }
    void setGoal(double goal) { goal_ = goal; }


private:
    int         repeatCount_{1};    ///< 事件触发后的重复次数
    EDirection  direction_{eBoth};  ///< 事件检测开关函数的方向
    double      threshold_{1e-10};  ///< 事件检测开关函数的阈值
    double      goal_{0.0};         ///< 事件检测的目标值
};

/// @brief 泛型ODE积分的事件检测器
/// @details 
template<typename Func>
class ODEEventDetectorGeneric: public ODEEventDetector
{
public:
    explicit ODEEventDetectorGeneric(Func func) 
        : func_(std::move(func)) 
    {}
    
    double getValue(const double* y,double x) const override {
        return func_(y, x);
    }
    
private:
    Func func_;
};


AST_NAMESPACE_END
