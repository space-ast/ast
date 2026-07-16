///
/// @file      BlockSRPPartial.cpp
/// @brief     太阳辐射压力偏导数（梯度）函数块实现
/// @details   继承 BlockSRP，重写 run() 以同时计算加速度和偏导数
/// @author    axel
/// @date      2026-07-12
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

#include "BlockSRPPartial.hpp"
#include "AstCore/BlockAstro.hpp"
#include "AstCore/CelestialBody.hpp"
#include "AstCore/BodyPosition.hpp"
#include "AstCore/Frame.hpp"
#include "AstCore/BuiltinFrame.hpp"
#include "AstUtil/IdentifierAPI.hpp"
#include "AstUtil/Constants.h"
#include "AstMath/Vector.hpp"
#include "AstMath/MathOperator.hpp"
#include "AstCore/NoneEclipseCalculator.hpp"

AST_NAMESPACE_BEGIN

using namespace math;

BlockSRPPartial::BlockSRPPartial()
    : BlockSRP{}
{
    init();
}

BlockSRPPartial::BlockSRPPartial(EclipseCalculator* eclipseCalculator, double cr, double srpArea, Frame* propagationFrame)
    : BlockSRP(eclipseCalculator, cr, srpArea, propagationFrame)
{
    init();
}

void BlockSRPPartial::init()
{
    static auto identifierAMatrix = aIdentifier(kIdentifierAMatrix);
    static auto identAccSensSRP = aIdentifier(kIdentifierAccSensitivityWrtSRP);
    outputPorts_.insert(outputPorts_.end(), {
        {identifierAMatrix, (signal_t*)&aMatrixPtr_, 36, DataPort::eDouble, DataPort::eAccumulate},
        {identAccSensSRP, (signal_t*)&accSensitivityToSRP_, 3, DataPort::eDouble}
    });
}

errc_t BlockSRPPartial::run(const SimTime& simTime)
{
    if (A_UNLIKELY(!eclipseCalculator_) || A_UNLIKELY(!propagationFrame_))
    {
        aError("BlockSRPPartial: eclipseCalculator or propagationFrame is null");
        *accSRP_ = Vector3d::Zero();
        if (useSRPSensitivity_)
            *accSensitivityToSRP_ = Vector3d::Zero();
        return eErrorNullInput;
    }

    // ── 复用 BlockSRP 的 SRP 加速度计算逻辑 ──
    // @todo 这里的计算逻辑和 BlockSRP 中的 run() 重复，需要考虑怎么复用计算逻辑

    auto& tp = simTime.timePoint();
    auto sun = eclipseCalculator_->lightSource();
    if (A_UNLIKELY(!sun))
    {
        aError("BlockSRPPartial: sun (light source) is null");
        *accSRP_ = Vector3d::Zero();
        if (useSRPSensitivity_)
            *accSensitivityToSRP_ = Vector3d::Zero();
        return eErrorNullInput;
    }

    double lightingRatio = eclipseCalculator_->getLightingRatio(tp, *position_, propagationFrame_);
    if (lightingRatio == 0)
    {
        *accSRP_ = Vector3d::Zero();
        // 这里不能直接返回，因为 accSensitivityToSRP_ 也需要设置为零
        if (useSRPSensitivity_) 
        {
            *accSensitivityToSRP_ = Vector3d::Zero();
        }
        return eNoError;
    }

    Vector3d scToSun;  // 航天器指向太阳的向量

    const EAberrationFlags aberrationFlags = EAberrationFlags::eCN_S;
    errc_t err;
    if (sunPosition_ == ESunPosition::eApparent)
    {
        err = aApparentPositionInFrame(sun, tp, propagationFrame_, *position_, *velocity_, aberrationFlags, scToSun, nullptr);
    }
    else if (sunPosition_ == ESunPosition::eApparentSunToTrueCB)
    {
        Vector3d sunPos;
        err = aApparentPositionInFrame(sun, tp, propagationFrame_, Vector3d::Zero(), Vector3d::Zero(), aberrationFlags, sunPos, nullptr);
        scToSun = sunPos - *position_;
    }
    else // if(sunPosition_ == ESunPosition::eTrue)
    {
        Vector3d sunPos;
        err = sun->getPosIn(propagationFrame_, tp, sunPos);
        scToSun = sunPos - *position_;
    }

    if (A_UNLIKELY(err != eNoError))
    {
        aError("failed to calculate sun position");
        *accSRP_ = Vector3d::Zero();
        if (useSRPSensitivity_)
        {
            *accSensitivityToSRP_ = Vector3d::Zero();
        }
        return err;
    }

    double rSqr = scToSun.squaredNorm();
    double r = std::sqrt(rSqr);

    double mass = *mass_;
    if (A_UNLIKELY(mass <= 0))
    {
        aError("spacecraft mass is zero or negative (%f), cannot compute SRP acceleration", mass);
        *accSRP_ = Vector3d::Zero();
        if (useSRPSensitivity_)
        {
            *accSensitivityToSRP_ = Vector3d::Zero();
        }
        return eErrorInvalidParam;
    }

    // 太阳辐射压力加速度
    // a_srp = Cr * (A/m) * P_1AU * (AU/r)^2 * (sunToSc / r)
    //       = Cr * A/m * P_1AU * AU^2 / r^3 * sunToSc
    // 方向：远离太阳（从太阳指向航天器）
    double factor = -lightingRatio * cr_ * srpArea_ / mass * kSolarPressureAt1AU * (kAU * kAU) / (r * rSqr);
    *accSRP_ = factor * scToSun;

    // 添加到速度导数上
    *velocityDerivative_ += *accSRP_;

    // ── 计算偏导数 ∂a_srp/∂r，累加到 A 矩阵 ──
    //
    // a_srp = factor · scToSun
    // 其中 factor = β/r³,  β = -ν·Cr·A/m·P₀·AU²
    //
    // 推导：
    //   ∂a_srp_i/∂pos_j = ∂(factor·s_i)/∂s_k · ∂s_k/∂pos_j
    //   其中 s = scToSun = sunPos - position,  ∂s_k/∂pos_j = -δ_kj
    //
    //   ∂a_srp_i/∂pos_j = -∂(factor·s_i)/∂s_j
    //   = -[∂factor/∂s_j · s_i + factor · δ_ij]
    //   = -[(-3β/r⁴)·(s_j/r)·s_i + factor·δ_ij]
    //   = -[(-3·factor/r)·(s_j/r)·s_i + factor·δ_ij]    （因为 β/r³ = factor, -3β/r⁴ = -3·factor/r）
    //   = -[-3·factor·s_i·s_j/r² + factor·δ_ij]
    //   = factor · (3·s_i·s_j/r² - δ_ij)
    //
    // 矩阵形式：
    //   ∂a_srp/∂r = factor · (3·scToSun·scToSunᵀ/r² − I₃)
    //
    {
        double invRSqr = 1.0 / rSqr;
        Matrix6d& A = *aMatrixPtr_;

        // 累加 ∂a_srp/∂r 到 A(3:5, 0:2)
        for (int i = 0; i < 3; ++i)
        {
            for (int j = 0; j < 3; ++j)
            {
                double dij = (i == j) ? 1.0 : 0.0;
                double da_dr = factor * (3.0 * scToSun[i] * scToSun[j] * invRSqr - dij);
                A(3 + i, j) += da_dr;
            }
        }
    }

    // SRP 综合参数 K 敏感度强迫项: ∂a_srp/∂K
    // K = Cr·A/m，强迫项为 3维加速度向量
    // a_srp = -ν · K · P₁AU · AU² / r³ · scToSun
    // ∂a_srp/∂K = -ν · P₁AU · AU² / r³ · scToSun  （与 K 无关，直接计算避免除零）
    if (useSRPSensitivity_) {
        double da_dK = -lightingRatio * kSolarPressureAt1AU * (kAU * kAU) / (r * rSqr);
        *accSensitivityToSRP_ = da_dK * scToSun;
    }

    return eNoError;
}

AST_NAMESPACE_END
