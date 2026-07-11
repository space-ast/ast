///
/// @file      BlockGravityPartial.cpp
/// @brief     重力场偏导数（梯度）函数块实现
/// @details   继承 BlockGravity，重写 run() 以同时计算加速度和梯度
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

#include "BlockGravityPartial.hpp"
#include "AstCore/BlockAstro.hpp"
#include "AstCore/FrameTransform.hpp"
#include "AstMath/Vector.hpp"
#include "AstMath/Matrix.hpp"
#include "AstMath/MathOperator.hpp"
#include "AstUtil/IdentifierAPI.hpp"

AST_NAMESPACE_BEGIN

using namespace math;

BlockGravityPartial::BlockGravityPartial()
    : BlockGravity{}
{
    init();
}

BlockGravityPartial::BlockGravityPartial(const GravityField &gravityField, int degree, int order,
                                         Axes *gravityAxes, Axes *propagationAxes)
    : BlockGravity(gravityField, degree, order, gravityAxes, propagationAxes)
{
    init();
}

BlockGravityPartial::BlockGravityPartial(GravityField &&gravityField, int degree, int order,
                                         Axes *gravityAxes, Axes *propagationAxes)
    : BlockGravity(std::move(gravityField), degree, order, gravityAxes, propagationAxes)
{
    init();
}

void BlockGravityPartial::init()
{
    static auto identifierAMatrix = aIdentifier(kIdentifierAMatrix);
    inputPorts_.push_back({identifierAMatrix, (signal_t*)&aMatrixPtr_, 36, DataPort::eDouble});
}


errc_t BlockGravityPartial::run(const SimTime& simTime)
{
    Rotation rotation;
    errc_t rc = prepare(simTime, rotation);
    if (A_UNLIKELY(rc != eNoError))
    {
        aError("failed to transform from propagation axes to gravity axes");
        *accGravityPtr_ = Vector3d::Zero();
        return rc;
    }

    Vector3d posInGravityAxes;      // 位置(重力坐标系下)
    Vector3d accInGravityAxes;      // 重力加速度(重力坐标系下)
    Matrix3d gradientInGravityAxes; // 重力梯度张量(重力坐标系下)
    // 将位置变换预报坐标系到重力场坐标系
    posInGravityAxes = rotation.transformVector(*posPtr_);
    // 计算总加速度和梯度张量（中心天体 + 摄动，重力坐标系下）
    gravityCalculator_.calcTotalAcceleration(posInGravityAxes, accInGravityAxes, gradientInGravityAxes);
    // 将加速度变换到预报坐标系
    Vector3d accInPropagationAxes = rotation.transformVectorInv(accInGravityAxes);
    *accGravityPtr_ = accInPropagationAxes;
    *velocityDerivativePtr_ += accInPropagationAxes;

    // 将梯度张量变换到预报坐标系：H_prop = Rᵀ · H_grav · R
    Matrix3d& R = rotation.getMatrix();
    Matrix3d R_T = R.transpose();
    Matrix3d gradientInPropAxes = R_T * gradientInGravityAxes * R;

    // 累加到 A 矩阵的 bottom-left 3×3（rows 3-5, cols 0-2）
    Matrix6d& A = *aMatrixPtr_;
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            A(3 + i, j) += gradientInPropAxes(i, j);

    return eNoError;
}


AST_NAMESPACE_END
