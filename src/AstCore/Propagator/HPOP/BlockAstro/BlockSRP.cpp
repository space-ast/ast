///
/// @file      BlockSRP.cpp
/// @brief     太阳辐射压力函数块实现
/// @details   ~
/// @author    axel
/// @date      2026-01-19
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

#include "BlockSRP.hpp"
#include "AstCore/BlockAstro.hpp"
#include "AstCore/CelestialBody.hpp"
#include "AstCore/Frame.hpp"
#include "AstUtil/Identifier.hpp"
#include "AstUtil/Constants.h"
#include "AstMath/Vector.hpp"
#include "AstMath/MathOperator.hpp"

AST_NAMESPACE_BEGIN

using namespace math;

/// @brief 1 AU处的太阳辐射压力 [N/m²]，使用太阳光度计算得到
/// @details
/// 由太阳光度推导：
///   Φ(1AU) = L_sun / (4π · AU²)          — 1AU处的辐射通量 [W/m²]
///   P_1AU  = Φ(1AU) / c                  — 1AU处的辐射压力 [N/m²]
///          = L_sun / (4π · AU² · c)
///
/// 代入：
///   L_sun = 3.839×10²⁶ W (Carrol & Ostlie, 2007)
///   AU    = 1.49597870691×10¹¹ m
///   c     = 2.99792458×10⁸ m/s
///
///   P_1AU ≈ 4.553×10⁻⁶ N/m²
constexpr double kSolarPressureAt1AU_FromLuminosity = kSunLuminosity / (4.0 * kPI * kAU * kAU * kLightSpeed);

/// @brief 1 AU处的太阳辐射压力 [N/m²]，使用太阳常数计算得到
/// @details
///   P_1AU = Φ(1AU) / c ≈ 1367 / 2.99792458e8 ≈ 4.560e-6 N/m²
constexpr double kSolarPressureAt1AU_FromSolarConstant = kSolarConstant / kLightSpeed;

/// @brief 1 AU处的太阳辐射压力 [N/m²]，目前使用光度计算的值，也可以切换为使用太阳常数计算的值
constexpr double kSolarPressureAt1AU = kSolarPressureAt1AU_FromLuminosity;  


BlockSRP::BlockSRP()
    : BlockSRP(nullptr, 1.0, 20.0, nullptr)
{
}

BlockSRP::BlockSRP(CelestialBody* sun, double cr, double srpArea, Frame* propagationFrame)
    : BlockDerivative{}
    , posPropagation_{&vectorBuffer_}
    , accSRP_{&vectorBuffer_}
    , velocityDerivative_{&vectorBuffer_}
    , vectorBuffer_{}
    , sun_{sun}
    , cr_{cr}
    , srpArea_{srpArea}
    , propagationFrame_{propagationFrame}
{
    static auto identifierPos = aIdentifier(kIdentifierPos);
    static auto identifierAccSRP = aIdentifier(kIdentifierAccSRP);
    static auto identifierVel = aIdentifier(kIdentifierVel);
    static auto identifierMass = aIdentifier(kIdentifierMass);

    inputPorts_ = {
        // 位置（预报坐标系下，以主要天体为参考）
        {
            identifierPos,
            (signal_t*)&posPropagation_,
            3,
            DataPort::eDouble
        },
        // 质量
        {
            identifierMass,
            (signal_t*)&mass_,
            1,
            DataPort::eDouble
        }
    };

    outputPorts_ = {
        // 太阳辐射压力加速度
        {
            identifierAccSRP,
            (signal_t*)&accSRP_,
            3,
            DataPort::eDouble
        }
    };

    derivativePorts_ = {
        // 速度导数
        {
            identifierVel,
            (signal_t*)&velocityDerivative_,
            3,
            DataPort::eDouble
        }
    };
}

errc_t BlockSRP::run(const SimTime& simTime)
{
    assert(sun_);
    assert(propagationFrame_);

    auto& tp = simTime.timePoint();

    // 获取太阳在预报坐标系下的位置
    Vector3d sunPos;
    errc_t err = sun_->getPosIn(propagationFrame_, tp, sunPos);
    if (A_UNLIKELY(err != eNoError))
    {
        aError("failed to get sun position");
        return err;
    }

    // 太阳到航天器的向量
    Vector3d sunToSc = *posPropagation_ - sunPos;
    double rSqr = sunToSc.squaredNorm();
    double r = std::sqrt(rSqr);

    // 太阳辐射压力加速度
    // a_srp = Cr * (A/m) * P_1AU * (AU/r)^2 * (sunToSc / r)
    //       = Cr * A/m * P_1AU * AU^2 / r^3 * sunToSc
    // 方向：远离太阳（从太阳指向航天器）
    double factor = cr_ * srpArea_ / (*mass_) * kSolarPressureAt1AU * (kAU * kAU) / (r * rSqr);
    *accSRP_ = factor * sunToSc;

    // 添加到速度导数上
    *velocityDerivative_ += *accSRP_;

    return eNoError;
}

AST_NAMESPACE_END
