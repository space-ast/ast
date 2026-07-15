///
/// @file      BlockThirdBodyGravityPartial.cpp
/// @brief     三体球谐重力偏导数（梯度）函数块实现
/// @details   继承 BlockThirdBodyGravity，重写 run() 以同时计算加速度和梯度
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

#include "BlockThirdBodyGravityPartial.hpp"
#include "AstCore/BlockAstro.hpp"
#include "AstCore/FrameTransform.hpp"
#include "AstCore/CelestialBody.hpp"
#include "AstCore/RunTime.hpp"
#include "AstUtil/IdentifierAPI.hpp"
#include "AstUtil/Logger.hpp"
#include "AstMath/Vector.hpp"
#include "AstMath/Matrix.hpp"
#include "AstMath/MathOperator.hpp"

AST_NAMESPACE_BEGIN

using namespace math;

BlockThirdBodyGravityPartial::BlockThirdBodyGravityPartial()
    : BlockThirdBodyGravity{}
{
    init();
}

BlockThirdBodyGravityPartial::BlockThirdBodyGravityPartial(Point* thirdBody, GravityField&& gravityField,
                                                           int degree, int order, Axes* gravityAxes,
                                                           Frame* propagationFrame)
    : BlockThirdBodyGravity(thirdBody, std::move(gravityField), degree, order, gravityAxes, propagationFrame)
{
    init();
}

void BlockThirdBodyGravityPartial::init()
{
    static auto identifierAMatrix = aIdentifier(kIdentifierAMatrix);
    outputPorts_.push_back({identifierAMatrix, (signal_t*)&aMatrixPtr_, 36, DataPort::eDouble, DataPort::eAccumulate});
}

errc_t BlockThirdBodyGravityPartial::run(const SimTime& simTime)
{
    Vector3d thirdBodyPos;
    Rotation rotation;
    errc_t err = prepare(simTime, thirdBodyPos, rotation);
    if (A_UNLIKELY(err != eNoError))
        return err;

    // ── 1. 直接项（含梯度）──
    //    航天器相对第三体的位置: r_rel = r - r₃
    Vector3d relPos = (*posCBI) - thirdBodyPos;
    Vector3d relPosGravityAxes = rotation.transformVector(relPos);

    //    调用带梯度的 calcTotalAcceleration，一次性计算加速度和梯度张量
    Vector3d accGravityAxes;
    Matrix3d gradientInGravityAxes;
    gravityCalculator_.calcTotalAcceleration(relPosGravityAxes, accGravityAxes, gradientInGravityAxes);

    //    加速度逆变换回预报系
    Vector3d accDirect = rotation.transformVectorInv(accGravityAxes);

    //    梯度相似变换：H_prop = Rᵀ · H_grav · R
    Matrix3d& R = rotation.getMatrix();
    Matrix3d R_T = R.transpose();
    Matrix3d gradientInPropAxes = R_T * gradientInGravityAxes * R;

    // ── 2. 间接项（无需梯度，间接项不依赖航天器位置 r）──
    //    中心天体相对第三体的位置为 -r₃
    Vector3d centralBodyRelPos = -thirdBodyPos;
    Vector3d centralBodyRelPosGravityAxes = rotation.transformVector(centralBodyRelPos);
    Vector3d accIndirectGravityAxes;
    gravityCalculator_.calcTotalAcceleration(centralBodyRelPosGravityAxes, accIndirectGravityAxes);
    Vector3d accIndirect = rotation.transformVectorInv(accIndirectGravityAxes);

    // ── 3. 总摄动 = 直接项 − 间接项 ──
    Vector3d accTotal = accDirect - accIndirect;

    // 存储加速度并累加到速度导数
    *accThirdBody = accTotal;
    *velocityDerivative_ += accTotal;

    // ── 4. 将梯度累加到 A 矩阵的 bottom-left 3×3（rows 3-5, cols 0-2）──
    Matrix6d& A = *aMatrixPtr_;
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            A(3 + i, j) += gradientInPropAxes(i, j);

    return eNoError;
}

AST_NAMESPACE_END
