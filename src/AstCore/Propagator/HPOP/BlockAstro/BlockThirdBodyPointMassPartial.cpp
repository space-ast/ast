///
/// @file      BlockThirdBodyPointMassPartial.cpp
/// @brief     三体点质量引力偏导数（梯度）函数块实现
/// @details   继承 BlockThirdBodyPointMass，重写 run() 以同时计算加速度和梯度
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

#include "BlockThirdBodyPointMassPartial.hpp"
#include "AstCore/BlockAstro.hpp"
#include "AstCore/GravityCalculator.hpp"
#include "AstCore/Point.hpp"
#include "AstUtil/IdentifierAPI.hpp"
#include "AstMath/MathOperator.hpp"

AST_NAMESPACE_BEGIN

using namespace math;

BlockThirdBodyPointMassPartial::BlockThirdBodyPointMassPartial()
    : BlockThirdBodyPointMass{}
{
    init();
}

BlockThirdBodyPointMassPartial::BlockThirdBodyPointMassPartial(Point* thirdBody, double thirdBodyGM, Frame* propagationFrame)
    : BlockThirdBodyPointMass(thirdBody, thirdBodyGM, propagationFrame)
{
    init();
}

void BlockThirdBodyPointMassPartial::init()
{
    static auto identifierAMatrix = aIdentifier(kIdentifierAMatrix);
    outputPorts_.push_back({identifierAMatrix, (signal_t*)&aMatrixPtr_, 36, DataPort::eDouble, DataPort::eAccumulate});
}

errc_t BlockThirdBodyPointMassPartial::run(const SimTime& simTime)
{
    // 调用基类计算三体引力加速度（包含直接项和间接项），同时累加到速度导数
    errc_t err = BlockThirdBodyPointMass::run(simTime);
    if (A_UNLIKELY(err != eNoError))
    {
        return err;
    }

    // 获取第三天体位置，计算航天器相对第三天体的向量 d = r3 - r
    auto& tp = simTime.timePoint();
    Vector3d thirdBodyPos;
    err = thirdBody_->getPosIn(propagationFrame_, tp, thirdBodyPos);
    if (A_UNLIKELY(err != eNoError))
    {
        aError("failed to get third body position for partial derivative");
        return err;
    }
    Vector3d thirdBodyRelPos = thirdBodyPos - (*posCBI);  // d = r3 - r

    // 利用 aPointMassField 计算 Hessian 梯度张量
    // aPointMassField 的 Hessian 输出为：
    //   H = -GM/|d|^3 · I + 3·GM/|d|^5 · (d ⊗ d)
    // 这正是 ∂a_direct/∂r，间接项梯度为零
    Vector3d unusedAcc;
    Matrix3d hessian;
    aPointMassField(thirdBodyRelPos, thirdBodyGM_, unusedAcc, hessian);

    // 将 ∂a/∂r 累加到 A 矩阵的 bottom-left 3×3（rows 3-5, cols 0-2）
    Matrix6d& A = *aMatrixPtr_;
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            A(3 + i, j) += hessian(i, j);

    return eNoError;
}

AST_NAMESPACE_END
