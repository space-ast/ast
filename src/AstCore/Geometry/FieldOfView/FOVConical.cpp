///
/// @file      FOVConical.cpp
/// @brief     复杂圆锥视场类实现
/// @details   实现复杂圆锥视场的方法。
/// @author    axel
/// @date      2026-04-09
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
/// 本软件基于 Apache 2.0 开源许可证分发。
/// 您可在遵守许可证条款的前提下使用、修改和分发本软件。
/// 许可证全文请见：
///
///    http://www.apache.org/licenses/LICENSE-2.0
///
/// 重要须知：
/// 软件按"现有状态"提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。

#include "FOVConical.hpp"
#include "AstMath/Vector.hpp"
#include "AstUtil/Constants.h"
#include <algorithm>
#include <cmath>

AST_NAMESPACE_BEGIN

double FOVConical::angularMargin(const Vector3d& direction) const
{
    // 视轴 = +Z
    static const Vector3d kSensorBoresight{0.0, 0.0, 1.0};

    // 1. 锥角边界的余量（弧度）
    double angleRad = direction.angle(kSensorBoresight);
    double coneInnerMargin = angleRad - innerConeAngle_;
    double coneOuterMargin = outerConeAngle_ - angleRad;

    // 2. 时钟角余量（绕视轴的方位角，弧度）
    double clockRad = std::atan2(direction.y(), direction.x());
    if (clockRad < 0.0) { clockRad += 2.0 * kPI; }

    double clockMarginRad = 0.0;

    if (minClockAngle_ <= maxClockAngle_) {
        // 标准范围 [minClock, maxClock]
        if (clockRad < minClockAngle_) {
            clockMarginRad = clockRad - minClockAngle_;
        } else if (clockRad > maxClockAngle_) {
            clockMarginRad = maxClockAngle_ - clockRad;
        } else {
            clockMarginRad = std::min(clockRad - minClockAngle_, maxClockAngle_ - clockRad);
        }
    } else {
        // 跨越 0 rad 的范围，如 [350°, 10°] → [6.11, 0.17] rad
        double twoPi = 2.0 * kPI;
        if (clockRad >= minClockAngle_ || clockRad <= maxClockAngle_) {
            double marginToMin = (clockRad >= minClockAngle_)
                ? clockRad - minClockAngle_
                : (twoPi - minClockAngle_) + clockRad;
            double marginToMax = (clockRad <= maxClockAngle_)
                ? maxClockAngle_ - clockRad
                : maxClockAngle_ + (twoPi - clockRad);
            clockMarginRad = std::min(marginToMin, marginToMax);
        } else {
            double distToMin = minClockAngle_ - clockRad;
            double distToMax = clockRad - maxClockAngle_;
            clockMarginRad = -std::min(distToMin, distToMax);
        }
    }

    return std::min({coneInnerMargin, coneOuterMargin, clockMarginRad});
}

AST_NAMESPACE_END
