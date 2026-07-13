///
/// @file      BlockDragPartial.cpp
/// @brief     大气阻力偏导数（梯度）函数块实现
/// @details   继承 BlockDrag，重写 run() 以同时计算加速度和偏导数
/// @author    axel
/// @date      2026-07-11
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

#include "BlockDragPartial.hpp"
#include "AstCore/BlockAstro.hpp"
#include "AstCore/Frame.hpp"
#include "AstUtil/IdentifierAPI.hpp"
#include "AstMath/Vector.hpp"
#include "AstMath/Matrix.hpp"
#include "AstMath/KinematicTransform.hpp"
#include "AstMath/MathOperator.hpp"

AST_NAMESPACE_BEGIN

using namespace math;

namespace {

/// @brief 从角速度向量构造叉乘反对称矩阵 [ω×]
/// @details
///   [ω×] = [[0,   -ωz,  ωy],
///           [ωz,   0,  -ωx],
///           [-ωy,  ωx,  0 ]]
A_ALWAYS_INLINE Matrix3d crossMatrix(const Vector3d& w)
{
    return Matrix3d{
         0,   -w(2),  w(1),
         w(2),  0,   -w(0),
        -w(1),  w(0),  0
    };
}

} // anonymous namespace

BlockDragPartial::BlockDragPartial()
    : BlockDrag{}
{
    init();
}

BlockDragPartial::BlockDragPartial(Atmosphere* atmosphere, double dragCoefficient, double dragArea, Frame* propagationFrame)
    : BlockDrag(atmosphere, dragCoefficient, dragArea, propagationFrame)
{
    init();
}

void BlockDragPartial::init()
{
    static auto identifierAMatrix = aIdentifier(kIdentifierAMatrix);
    outputPorts_.push_back({identifierAMatrix, (signal_t*)&aMatrixPtr_, 36, DataPort::eDouble, DataPort::eAccumulate});
}

errc_t BlockDragPartial::run(const SimTime& simTime)
{
    if (A_UNLIKELY(!atmosphere_))
    {
        aError("BlockDragPartial: atmosphere is null, cannot compute drag acceleration");
        *accDrag_ = Vector3d::Zero();
        return eErrorNullInput;
    }
    auto& tp = simTime.timePoint();

    Vector3d accDrag;
    Vector3d relVelocity;
    double density;
    Vector3d omega;                        // 大气系相对预报系的角速度（预报坐标系下）
    Vector3d posInAtmosFrame;              // 大气坐标系下的位置
    Matrix3d rotToAtmos;                   // 从预报坐标系到大气坐标系的旋转矩阵

    // @todo 这里的计算逻辑和 BlockDrag 中的 run() 重复，需要考虑怎么复用计算逻辑
    {
        Vector3d _;
        Vector3d atmosVelocity;
        KinematicTransform transform;
        Frame* atmosFrame = atmosphere_->getFrame();
        propagationFrame_->getTransformTo(atmosFrame, tp, transform);
        transform.transformPosition(*position_, posInAtmosFrame);

        // 保存角速度和旋转矩阵，后续用于计算偏导数
        omega = transform.getRotationRate();
        rotToAtmos = transform.getRotation().getMatrix();

        // 计算大气密度
        density = atmosphere_->getDensity(tp, posInAtmosFrame);
        transform.inverse().transformPositionVelocity(posInAtmosFrame, Vector3d::Zero(), _, atmosVelocity);

        // 计算航天器相对于大气的速度
        relVelocity = *velocity_ - atmosVelocity;

        // 计算阻力加速度: -½·Cd·A/m·ρ·|v_rel|·v_rel
        double mass = *mass_;
        if (A_UNLIKELY(mass <= 0))
        {
            aError("spacecraft mass is zero or negative (%f), cannot compute drag acceleration", mass);
            *accDrag_ = Vector3d::Zero();
            return eErrorInvalidParam;
        }
        accDrag = -dragCoefficient_ * dragArea_ * density * relVelocity.norm() / (mass * 2) * relVelocity;
    }

    // 写入输出端口和导数端口
    *accDrag_ = accDrag;
    *velocityDerivative_ += accDrag;

    // 计算偏导数并累加到 A 矩阵
    double relSpeed = relVelocity.norm();
    if (A_LIKELY(relSpeed > 0))
    {
        double k = dragCoefficient_ * dragArea_ / (*mass_ * 2);

        // 计算矩阵 D = v_rel·v_relᵀ / |v_rel| + |v_rel|·I
        //   D 是 |v_rel|·v_rel 对 v_rel 的雅可比矩阵（3×3）
        //   令 f(v) = |v| · v，分量形式 f_i = |v| · v_i，对 v_j 求偏导：
        //       ∂f_i/∂v_j = ∂(|v|)/∂v_j · v_i + |v| · ∂v_i/∂v_j
        //   其中 ∂(|v|)/∂v_j = v_j / |v|，∂v_i/∂v_j = δ_ij（单位矩阵），代入：
        //       ∂f_i/∂v_j = v_i·v_j / |v| + |v|·δ_ij
        //   写成矩阵形式就是：
        //       D = v_rel·v_relᵀ / |v_rel| + |v_rel|·I
        Matrix3d D;
        for (int i = 0; i < 3; ++i)
        {
            for (int j = 0; j < 3; ++j)
            {
                D(i, j) = relVelocity[i] * relVelocity[j] / relSpeed;
                if (i == j)
                    D(i, j) += relSpeed;
            }
        }

        Matrix6d& A = *aMatrixPtr_;

        // ∂a/∂v = -k·ρ·D → 累加到 A 矩阵 rows 3-5, cols 3-5
        double factorV = -k * density;
        for (int i = 0; i < 3; ++i)
            for (int j = 0; j < 3; ++j)
                A(3 + i, 3 + j) += factorV * D(i, j);

        // 大气旋转项
        constexpr bool useAtmosphereRotationTerm = true;
        if(useAtmosphereRotationTerm)
        {
            // ∂a/∂r 大气旋转项：k·ρ·D·[ω×] → 累加到 A 矩阵 rows 3-5, cols 0-2
            // 航天器位置改变 → 该处大气旋转线速度不同 → 相对速度改变 → 阻力加速度改变
            // ∂(v_rel)/∂r = ∂(v - ω×r)/∂r = -[ω×]
            // ∂a_drag/∂r = ∂a_drag/∂(v_rel) · ∂(v_rel)/∂r
            //            = (-k·ρ·D) · (-[ω×])
            //            = k·ρ·D·[ω×]
            Matrix3d omegaCross = crossMatrix(omega);
            Matrix3d D_omegaCross = D * omegaCross;
            double factorR = k * density;
            for (int i = 0; i < 3; ++i)
                for (int j = 0; j < 3; ++j)
                    A(3 + i, j) += factorR * D_omegaCross(i, j);
        }

        // 密度梯度项（通过高度有限差分计算）
        Matrix3d daDrDensity = computeDensityGradientContribution(relVelocity, relSpeed, k, tp, posInAtmosFrame, density, rotToAtmos);
        for (int i = 0; i < 3; ++i)
            for (int j = 0; j < 3; ++j)
                A(3 + i, j) += daDrDensity(i, j);
    }

    return eNoError;
}

Matrix3d BlockDragPartial::computeDensityGradientContribution(const Vector3d& relVelocity, double relSpeed, double k,
    const TimePoint& tp, const Vector3d& posInAtmosFrame, double density, const Matrix3d& rotToAtmos)
{
    // 密度梯度对 ∂a/∂r 的贡献公式：
    //   ∂a/∂r 密度项 = -k · |v_rel| · v_rel · (∂ρ/∂r)ᵀ
    //
    // 采用高度方向前向有限差分近似 ∂ρ/∂r：
    //   1. 沿径向（高度方向）扰动位置
    //   2. 计算扰动后的密度
    //   3. dρ/dh ≈ (ρ_pert - ρ) / h
    //   4. gradRho_atmos = dρ/dh · r_hat（大气坐标系下）
    //   5. gradRho_prop = Rᵀ · gradRho_atmos（变换到预报坐标系）
    //   6. 外积：u = -k · |v_rel| · v_rel，M = u · gradRho_propᵀ

    // 径向单位向量（大气坐标系下，即高度方向）
    double r = posInAtmosFrame.norm();
    if (r < 1e-3)
        return Matrix3d::Zero(); // 地心附近，无意义

    Vector3d rHat = posInAtmosFrame / r;

    // 前向有限差分：沿径向扰动 1 米
    constexpr double h = 1.0;
    Vector3d posPerturbed = posInAtmosFrame + h * rHat;
    double densityPerturbed = atmosphere_->getDensity(tp, posPerturbed);

    // 密度对高度的导数
    double drho_dh = (densityPerturbed - density) / h;

    // 大气坐标系下的密度梯度（仅径向分量，因密度主要随高度变化）
    Vector3d gradRhoAtmos = rHat * drho_dh;

    // 变换到预报坐标系：gradRho_prop = Rᵀ · gradRho_atmos（梯度按逆旋转变换）
    Vector3d gradRhoProp = rotToAtmos.transpose() * gradRhoAtmos;

    // 贡献矩阵：u = -k · |v_rel| · v_rel，M = u · gradRho_propᵀ
    double coeff = -k * relSpeed;
    Matrix3d contrib;
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            contrib(i, j) = coeff * relVelocity[i] * gradRhoProp[j];

    return contrib;
}

AST_NAMESPACE_END
