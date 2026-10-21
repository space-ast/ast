///
/// @file      Gibbs.cpp
/// @brief     Gibbs 初轨确定实现
/// @details   由三个连续时刻的位置矢量求解中间时刻的速度
/// @author    axel
/// @date      2026-10-21
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

#include "Gibbs.hpp"

#include "AstMath/Vector.hpp"

#include <cmath>

AST_NAMESPACE_BEGIN

namespace
{

/// @brief 三点位置退化判据
/// @details 三个位置矢量共线或重合时 D→0，轨道面法向不唯一。
constexpr double kGibbsDegenerateTol = 1e-12;

}

errc_t aGibbs(const Vector3d& r1, const Vector3d& r2, const Vector3d& r3, double gm, Vector3d& v2)
{
    const double n1 = r1.norm();
    const double n2 = r2.norm();
    const double n3 = r3.norm();
    if (!(n1 > 0.0) || !(n2 > 0.0) || !(n3 > 0.0))
        return eErrorInvalidParam;

    // Gibbs 几何量（Curtis, Orbital Mechanics for Engineering Students, 5.4 节）：
    //   z12 = r1×r2, z23 = r2×r3, z31 = r3×r1
    //   D = z12 + z23 + z31              —— 与轨道面法向同向
    //   N = n1·z23 + n2·z31 + n3·z12     —— 与 D 同向，|N|/|D| 即半通径 p
    //   S = r1(n2−n3) + r2(n3−n1) + r3(n1−n2)
    const Vector3d z12 = r1.cross(r2);
    const Vector3d z23 = r2.cross(r3);
    const Vector3d z31 = r3.cross(r1);
    const Vector3d D   = z12 + z23 + z31;
    const Vector3d N   = z23 * n1 + z31 * n2 + z12 * n3;
    const Vector3d B   = D.cross(r2);
    const Vector3d S   = r1 * (n2 - n3) + r2 * (n3 - n1) + r3 * (n1 - n2);

    // 退化检查：共线/重合时 D→0，轨道面法向不唯一
    const double dNorm = D.norm();
    if (!(dNorm > kGibbsDegenerateTol))
        return eErrorInvalidValue;

    const double nNorm = N.norm();
    if (!(nNorm > 0.0))
        return eErrorInvalidValue;

    // v2 = √(μ/(|N||D|)) · (B/|r2| + S)
    // B/|r2| 是 r2 处横向单位矢量的 |D| 倍，S 提供径向与剩余横向分量
    const double vm = std::sqrt(gm / (nNorm * dNorm));
    v2 = B * (vm / n2) + S * vm;

    return eNoError;
}

AST_NAMESPACE_END
