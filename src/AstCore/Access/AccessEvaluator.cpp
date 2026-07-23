///
/// @file      AccessEvaluator.cpp
/// @brief     访问评估器实现
/// @details   步进采样 → 符号翻转检测 → Brent/二分法求根精化 → 区间合并
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
#include "AstUtil/Logger.hpp"
#include <cmath>

AST_NAMESPACE_BEGIN


constexpr double kZeroTol = 1e-9;  // 约束值零容差：浮点噪声可能产生 ~1e-10 偏差


/// @brief 评估访问约束的时间段，累加结果到 result 中
/// @param constraint 访问约束
/// @param stepper 步进策略
/// @param interval 搜索时间区间
/// @param result 满足约束的时间段列表，会累加结果，不会清空已有时段
/// @return 错误码
static errc_t aEvaluateAccess_AppendResult(
    const AccessConstraint* constraint,
    AccessStepper* stepper,
    const TimeInterval& interval,
    TimeIntervalList& result
)
{
    if (!constraint || !stepper) { return eErrorInvalidParam; }

    auto& start = interval.start();
    auto& stop  = interval.stop();

    // 初始化步进器
    stepper->init(start, stop);

    // 初始化结果列表
    if(result.empty())
        result.setEpoch(start);

    // 定义满足约束的判断函数
    auto isSatisfied = [](double v) { return v >= -kZeroTol; };

    double prevValue = constraint->evaluate(start);

    // 初始访问状态
    bool   inAccess  = isSatisfied(prevValue);
    TimePoint boundaryStart = start;

    TimePoint prevTime = start;
    TimePoint currentTime;

    while (stepper->next(currentTime)) {
        double currentValue = constraint->evaluate(currentTime);

        // 符号翻转检测（使用容差，容忍浮点噪声导致约束值不精确为零）
        if (isSatisfied(prevValue) != isSatisfied(currentValue)) {
            double dt = currentTime - prevTime;
            TimePoint refTime = prevTime;

            // 精化求根
            double rootT = 0.0;
            if (fabs(prevValue) <= kZeroTol || fabs(currentValue) <= kZeroTol) {
                // 约束值一端近零（平台状）：Brent 无法精化，改用二分法
                // neg: f(neg) < -kZeroTol, zero: f(zero) >= -kZeroTol
                double neg, zero;
                if (!isSatisfied(prevValue)) {
                    neg = 0;    zero = dt;
                } else {
                    neg = dt;   zero = 0;
                }
                constexpr double kTol = 1e-4; // 时间精度[s]
                while (fabs(zero - neg) > kTol) {
                    double mid = (neg + zero) * 0.5;
                    double fmid = constraint->evaluate(refTime + mid);
                    if (isSatisfied(fmid)) {
                        zero = mid;
                    } else {
                        neg  = mid;
                    }
                }
                rootT = zero; // 取非负端
            } else {
                // 两端都非零：正常 Brent 求根
                BrentSolver().solve([constraint, refTime](double x) -> double {
                    return constraint->evaluate(refTime + x);
                }, 0.0, dt, rootT);
            }

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

    // 处理最后一个区间
    if (inAccess) {
        result.push_back(boundaryStart, stop);
    }

    return eNoError;
}


errc_t aEvaluateAccess(
    const AccessConstraint* constraint,
    AccessStepper* stepper,
    const TimeInterval& interval,
    TimeIntervalList& result
)
{
    result.clear();
    return aEvaluateAccess_AppendResult(constraint, stepper, interval, result);
}


errc_t aEvaluateAccess(
    const AccessConstraint* constraint,
    AccessStepper* stepper,
    const TimeIntervalList& intervalList,
    TimeIntervalList& result
)
{
    // 采用函数局部变量存储结果，避免intervalList和result的地址相同导致的异常行为
    TimeIntervalList satisfiedIntervalList;
    errc_t rc = eNoError;
    for (const auto& interval : intervalList) {
        errc_t err = aEvaluateAccess_AppendResult(constraint, stepper, interval, satisfiedIntervalList);
        if (err != eNoError) {
            rc = err;
            aError("failed to evaluate access for interval: %s", interval.toString().c_str());
        }
    }
    result = std::move(satisfiedIntervalList);
    return rc;
}


errc_t aEvaluateAccess(
    const std::vector<AccessConstraint*>& constraints,
    AccessStepper* stepper,
    const TimeInterval& interval,
    TimeIntervalList& result
)
{
    if(constraints.empty())
    {
        // 没有约束，直接返回输入的时间区间
        result = TimeIntervalList::FromTimeInterval(interval);
        aInfo("no constraints, return the input interval: %s", interval.toString().c_str());
        return eNoError;
    }
    errc_t rc = aEvaluateAccess(constraints[0], stepper, interval, result);
    for(size_t i=1; i<constraints.size(); i++)
    {
        rc |= aEvaluateAccess(constraints[i], stepper, result, result);
    }
    return rc;
}


AccessEvaluator::AccessEvaluator()  = default;
AccessEvaluator::~AccessEvaluator() = default;

bool AccessEvaluator::check(const TimePoint& time) const
{
    if (!constraint_) { return false; }
    return constraint_->evaluate(time) >= 0.0;
}

errc_t AccessEvaluator::evaluate(const TimeInterval& interval, TimeIntervalList& result)
{
    return aEvaluateAccess(constraint_, stepper_, interval, result);
}

AST_NAMESPACE_END
