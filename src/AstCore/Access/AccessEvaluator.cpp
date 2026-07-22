///
/// @file      AccessEvaluator.cpp
/// @brief     访问评估器实现
/// @details   步进采样 → 符号翻转检测 → Brent 求根精化 → 区间合并
/// @author    axel
/// @date      2026-07-22
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
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

#include "AccessEvaluator.hpp"
#include "AccessStepper.hpp"
#include "AstCore/AccessConstraint.hpp"
#include "AstMath/BrentSolver.hpp"

AST_NAMESPACE_BEGIN

AccessEvaluator::AccessEvaluator()  = default;
AccessEvaluator::~AccessEvaluator() = default;

bool AccessEvaluator::check(const TimePoint& time) const
{
    if (!constraint_) { return false; }
    return constraint_->evaluate(time) > 0.0;
}

errc_t AccessEvaluator::evaluate(const TimeInterval& interval, TimeIntervalList& result)
{
    return aEvaluateAccess(constraint_, stepper_, interval, result);
}

errc_t aEvaluateAccess(
    const AccessConstraint* constraint,
    AccessStepper* stepper,
    const TimeInterval& interval,
    TimeIntervalList& result)
{
    if (!constraint || !stepper) { return eErrorInvalidParam; }

    auto start = interval.start();
    auto stop  = interval.stop();

    stepper->init(start, stop);
    result.setEpoch(start);

    double prevValue = constraint->evaluate(start);
    bool   inAccess  = (prevValue > 0.0);
    TimePoint boundaryStart = start;

    TimePoint prevTime = start;
    TimePoint currentTime;

    while (stepper->next(currentTime)) {
        double currentValue = constraint->evaluate(currentTime);

        // 符号翻转检测
        if ((prevValue > 0.0) != (currentValue > 0.0)) {
            double dt = currentTime - prevTime;
            TimePoint refTime = prevTime;

            // Brent 精化求根
            double rootT = 0.0;
            BrentSolver().solve([constraint, refTime](double x) -> double {
                return constraint->evaluate(refTime + x);
            }, 0.0, dt, rootT);

            TimePoint rootTime = refTime + rootT;

            if (inAccess) {
                result.push_back(boundaryStart, rootTime);
            }

            boundaryStart = rootTime;
            inAccess = !inAccess;
        }

        prevTime  = currentTime;
        prevValue = currentValue;
    }

    if (inAccess) {
        result.push_back(boundaryStart, stop);
    }

    return eNoError;
}

AST_NAMESPACE_END
