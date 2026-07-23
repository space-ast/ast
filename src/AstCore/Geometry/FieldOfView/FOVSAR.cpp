///
/// @file      FOVSAR.cpp
/// @brief     合成孔径雷达视场类实现
/// @details   实现合成孔径雷达视场的方法
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

#include "FOVSAR.hpp"
#include "AstMath/Vector.hpp"
#include <algorithm>
#include <cmath>

AST_NAMESPACE_BEGIN

double FOVSAR::angularMargin(const Vector3d& direction) const
{
    // 视轴 = +Z
    // 仰角 = atan2(y, z)，方位角 = atan2(x, z)
    double elevRad = std::atan2(direction.y(), direction.z());
    double azimRad = std::atan2(direction.x(), direction.z());

    // 仰角余量
    double elevMinMargin = elevRad - minElevAngle_;
    double elevMaxMargin = maxElevAngle_ - elevRad;

    // 方位排除区余量（前向和后向区域不可见）
    double absAzim = std::abs(azimRad);
    double forwardMargin  = absAzim - forwardExcludeAngle_;
    double backwardMargin = (kPI - absAzim) - backwardExcludeAngle_;

    return std::min({elevMinMargin, elevMaxMargin, forwardMargin, backwardMargin});
}

AST_NAMESPACE_END
