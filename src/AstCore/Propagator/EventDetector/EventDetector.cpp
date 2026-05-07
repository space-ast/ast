///
/// @file      EventDetector.cpp
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

#include "EventDetector.hpp"
#include "AstMath/ODEEventDetector.hpp"
#include "AstMath/Bracket.hpp"
#include "AstCore/SpacecraftState.hpp"
#include "AstUtil/Logger.hpp"
#include "AstUtil/Math.hpp"

AST_NAMESPACE_BEGIN

namespace{

/// @brief 事件检测器包装类
/// 该类用于包装事件检测器实例，将其转换为ODE事件检测器包装类
class ODEEventDetectorWrap: public ODEEventDetector
{
public:
    ODEEventDetectorWrap(EventDetector* eventDetector)
        : eventDetector_(eventDetector)
    {
        spacecraftState_ = SpacecraftState::NewDefault();
    }
    ~ODEEventDetectorWrap() override = default;
    double getValue(const double* y, double x) const override
    {
        // @todo 这里需要处理其他一般情况
        CartState* cartState = (CartState*)y;
        spacecraftState_->getOrbitState()->setState(*cartState);
        return eventDetector_->getValue(*spacecraftState_, x);
    }

private:
    SharedPtr<SpacecraftState> spacecraftState_;    ///< 航天状态实例指针
    SharedPtr<EventDetector> eventDetector_;        ///< 事件检测器实例指针
};


class ODEEventDetectorWrapForAngle: public ODEEventDetectorWrap
{
    using ODEEventDetectorWrap::ODEEventDetectorWrap;
public:
    /*
    对于角度量，需要实现：
    1. 使不同周期的角度量都能够触发事件
    2. 需要过滤掉每个周期内的函数值跳变
    */

    double getDifference(const double* y,double x) const override
    {
        double difference = getValue(y, x) - getGoal();
        return aNormalizeAngleNegPiToPi(difference);
    }
    bool containsEvent(const Bracket& bracket) const override
    {
        double delta = bracket.rightValue() - bracket.leftValue();
        if(fabs(delta) < kPI)
            return ODEEventDetector::containsEvent(bracket);
        return false;
    }

};

}


ODEEventDetector* EventDetector::newODEEventDetector() const
{
    // 这里需要根据是否为角度量来选择不同的包装类
    bool isAngle = this->isAngle();
    ODEEventDetectorWrap* odeEventDetector;
    if(isAngle)
    {
        odeEventDetector = new ODEEventDetectorWrapForAngle(const_cast<EventDetector*>(this));
    }
    else
    {
        odeEventDetector = new ODEEventDetectorWrap(const_cast<EventDetector*>(this));
    }
    odeEventDetector->setRepeatCount(repeatCount_);
    odeEventDetector->setGoal(goal_);
    odeEventDetector->setDirection(direction_);
    odeEventDetector->setThreshold(threshold_);
    return odeEventDetector;
}

AST_NAMESPACE_END
