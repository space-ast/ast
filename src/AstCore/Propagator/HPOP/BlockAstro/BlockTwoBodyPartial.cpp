///
/// @file      BlockTwoBodyPartial.cpp
/// @brief     二体引力偏导数（梯度）函数块实现
/// @details   继承 BlockTwoBody，重写 run() 以同时计算加速度和梯度
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

#include "BlockTwoBodyPartial.hpp"
#include "AstCore/BlockAstro.hpp"
#include "AstCore/GravityCalculator.hpp"
#include "AstUtil/IdentifierAPI.hpp"
#include "AstMath/MathOperator.hpp"

AST_NAMESPACE_BEGIN

using namespace math;

BlockTwoBodyPartial::BlockTwoBodyPartial()
    : BlockTwoBody{}
{
    init();
}

BlockTwoBodyPartial::BlockTwoBodyPartial(double twoBodyGM)
    : BlockTwoBody(twoBodyGM)
{
    init();
}

void BlockTwoBodyPartial::init()
{
    static auto identifierAMatrix = aIdentifier(kIdentifierAMatrix);
    outputPorts_.push_back({identifierAMatrix, (signal_t*)&aMatrixPtr_, 36, DataPort::eDouble, DataPort::eAccumulate});
}

errc_t BlockTwoBodyPartial::run(const SimTime& simTime)
{
    // 使用 aPointMassField 同时计算二体引力加速度和 Hessian 张量
    Vector3d accCBI;
    Matrix3d hessian;
    aPointMassField(*posCBI, twoBodyGM_, accCBI, hessian);

    *accTwoBody = accCBI;
    *velocityDerivative_ += accCBI;

    // 将 Hessian 梯度张量 ∂a/∂r 累加到 A 矩阵的 bottom-left 3×3（rows 3-5, cols 0-2）
    Matrix6d& A = *aMatrixPtr_;
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            A(3 + i, j) += hessian(i, j);

    return eNoError;
}

AST_NAMESPACE_END
