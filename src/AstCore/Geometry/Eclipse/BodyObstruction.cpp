///
/// @file      BodyObstruction.cpp
/// @brief     天体遮挡判断实现
/// @details   计算视线到天体表面的最短有符号距离，支持球体和扁球体。
/// @author    axel
/// @date      2026-07-21
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

#include "BodyObstruction.hpp"
#include "AstCore/SphereShape.hpp"
#include "AstCore/SpheroidShape.hpp"
#include <algorithm>
#include <cmath>

AST_NAMESPACE_BEGIN

namespace
{

// 计算线段到原点的最短有符号距离（单位球体）
// 返回值：>0 线段在球外，<0 线段穿入球内
double lineSegmentClearanceToUnitSphere(const Vector3d& a, const Vector3d& b)
{
    Vector3d dir;
    dir.x() = b.x() - a.x();
    dir.y() = b.y() - a.y();
    dir.z() = b.z() - a.z();

    double dirLenSq = dir.x() * dir.x() + dir.y() * dir.y() + dir.z() * dir.z();

    if (dirLenSq < 1e-20) {
        // 两点重合，退化为点到球面的距离
        double distA = std::sqrt(a.x() * a.x() + a.y() * a.y() + a.z() * a.z());
        return distA - 1.0;
    }

    // 线段参数方程 P(t) = a + t·dir, t ∈ [0, 1]
    // 最近点参数 t = -(a·dir) / |dir|²
    double aDotDir = a.x() * dir.x() + a.y() * dir.y() + a.z() * dir.z();
    double tClosest = -aDotDir / dirLenSq;

    // 钳制到 [0, 1]
    if (tClosest < 0.0) tClosest = 0.0;
    if (tClosest > 1.0) tClosest = 1.0;

    // 最近点到球心的距离
    double px = a.x() + tClosest * dir.x();
    double py = a.y() + tClosest * dir.y();
    double pz = a.z() + tClosest * dir.z();

    double distToOrigin = std::sqrt(px * px + py * py + pz * pz);
    return distToOrigin - 1.0;
}

// 球体 clearance
double sphereClearance(const Vector3d& obs, const Vector3d& tgt, double radius)
{
    double invR = 1.0 / radius;
    Vector3d a{obs.x() * invR, obs.y() * invR, obs.z() * invR};
    Vector3d b{tgt.x() * invR, tgt.y() * invR, tgt.z() * invR};

    double unitClearance = lineSegmentClearanceToUnitSphere(a, b);
    return unitClearance * radius;
}

// 扁球体 clearance（仿射缩放法）
double spheroidClearance(const Vector3d& obs, const Vector3d& tgt,
                         double majorAxis, double flatFactor)
{
    // 扁球体半轴：(major, major, minor)，其中 minor = major·(1 - flatFactor)
    double minorAxis = majorAxis * (1.0 - flatFactor);

    if (minorAxis <= 0.0) { return -1.0; } // 退化情况

    double invMajor = 1.0 / majorAxis;
    double invMinor = 1.0 / minorAxis;

    // 仿射缩放：椭球 → 单位球，线段线性映射
    Vector3d a{obs.x() * invMajor, obs.y() * invMajor, obs.z() * invMinor};
    Vector3d b{tgt.x() * invMajor, tgt.y() * invMajor, tgt.z() * invMinor};

    double unitClearance = lineSegmentClearanceToUnitSphere(a, b);
    // 缩放回物理距离（用长轴作为近似）
    return unitClearance * majorAxis;
}

} // anonymous namespace

double aLineOfSightClearance(
    const Vector3d& observerPos,
    const Vector3d& targetPos,
    const BodyShape* bodyShape)
{
    if (!bodyShape) { return 1e12; } // 无天体 → 永无遮挡

    // 球体：直接使用半径
    if (auto* sphere = dynamic_cast<const SphereShape*>(bodyShape)) {
        return sphereClearance(observerPos, targetPos, sphere->radius_);
    }

    // 扁球体 / 旋转椭球体：仿射缩放法
    if (auto* spheroid = dynamic_cast<const SpheroidShape*>(bodyShape)) {
        return spheroidClearance(observerPos, targetPos,
                                 spheroid->majorAxis(), spheroid->flatFactor());
    }

    // 通用椭球体或其他形状：退化为等效球体
    return sphereClearance(observerPos, targetPos, bodyShape->majorAxis());
}

AST_NAMESPACE_END
