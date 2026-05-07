///
/// @file      EventDetector.hpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2026-01-27
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
#include "AstMath/ODEEventDetector.hpp"
#include "AstUtil/ObjectNamed.hpp"

AST_NAMESPACE_BEGIN


class SpacecraftState;

class ODEEventDetector;

/// @brief 事件检测基类
/// 事件检测基类，用于检测事件是否发生。
/// 参考orekit的EventDetector类
class AST_CORE_API EventDetector: public ObjectNamed
{
public:
    using EDirection = ODEEventDetector::EDirection;
    AST_OBJECT(EventDetector)
    AST_PROPERT(active)
    AST_PROPERT(repeatCount)
    AST_PROPERT(threshold)
    AST_PROPERT(goal)
    EventDetector() = default;
    virtual ~EventDetector() = default;

    /// @brief 获取事件检测开关函数与目标值的差值
    /// @param state 航天器状态
    /// @param t 时间（相对于预报器参考时间的秒数）
    /// @return 事件检测开关函数与目标值的差值
    virtual double getDifference(const SpacecraftState& state, double t) const{return getValue(state, t) - goal_;}

    /// @brief 获取事件检测开关函数的值
    /// @param state 航天器状态
    /// @param t 时间（相对于预报器参考时间的秒数）
    /// @return 事件检测开关函数的值
    virtual double getValue(const SpacecraftState& state, double t) const = 0;

    /// @brief 是否为角度事件检测器
    /// @return 是否为角度事件检测器
    virtual bool isAngle() const {return false;}
public:
    
    /// @brief 创建ODE事件检测器实例
    /// @return ODE事件检测器实例指针
    /// @note ODE事件检测器实例的生命周期由调用方负责管理，调用方需要在使用完成后调用析构函数释放资源
    ODEEventDetector* newODEEventDetector() const;
    
public: // rtti 暂时不支持枚举类型

    EDirection direction() const {return direction_;}
    void setDirection(EDirection dir) {direction_ = dir;}

PROPERTIES:

    bool active() const {return active_;}
    void setActive(bool active) {active_ = active;}

    int repeatCount() const {return repeatCount_;}
    void setRepeatCount(int count) {repeatCount_ = count;}


    double threshold() const {return threshold_;}
    void setThreshold(double threshold) {threshold_ = threshold;}

    double goal() const {return goal_;}
    void setGoal(double goal) {goal_ = goal;}

private:
    bool        active_{true};                  ///< 是否激活
    int         repeatCount_{1};                ///< 事件触发后的重复次数
    EDirection  direction_{EDirection::eBoth};  ///< 事件检测开关函数的方向
    double      threshold_{1e-10};              ///< 事件检测开关函数的阈值
    double      goal_{0.0};                     ///< 事件检测的目标值
};


/// @brief 泛型事件检测器
/// @details 
template<typename Func>
class EventDetectorGeneric: public EventDetector
{
public:
    explicit EventDetectorGeneric(Func func) 
        : func_(std::move(func)) 
    {}
    
    double getValue(const SpacecraftState& state, double t) const override {
        return func_(state, t);
    }
    
private:
    Func func_;
};


AST_NAMESPACE_END
