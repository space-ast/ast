///
/// @file      BlockStateTransitionMatrix.cpp
/// @brief     状态转换矩阵（STM）函数块实现
/// @details   实现 dΦ/dt = A(t) · Φ(t) 的矩阵乘法计算
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

#include "BlockStateTransitionMatrix.hpp"
#include "AstCore/BlockAstro.hpp"
#include "AstUtil/IdentifierAPI.hpp"
#include "AstMath/MathOperator.hpp"

AST_NAMESPACE_BEGIN

BlockStateTransitionMatrix::BlockStateTransitionMatrix()
    : BlockDerivative{}
{
    init();
}

void BlockStateTransitionMatrix::init()
{
    static auto identifierAMatrix = aIdentifier(kIdentifierAMatrix);
    static auto identifierSTM = aIdentifier(kIdentifierSTM);

    inputPorts_ = {
        // A 矩阵（动力学雅可比 ∂f/∂x，6×6 = 36 元素，行优先）
        {
            identifierAMatrix,
            (signal_t*)&aMatrixPtr_,
            36,
            DataPort::eDouble
        }
    };

    outputPorts_ = {
        // 状态转换矩阵 Φ（6×6 = 36 元素，行优先）
        // 该端口信号会被 BlockDynamicSystem::createStateMap() 重定向到状态向量
        // 积分器负责更新 Φ，本块在 run() 中读取 Φ 用于计算 dΦ/dt
        {
            identifierSTM,
            (signal_t*)&stmPtr_,
            36,
            DataPort::eDouble
        }
    };

    derivativePorts_ = {
        // Φ 的时间导数 dΦ/dt（6×6 = 36 元素，行优先）
        {
            identifierSTM,
            (signal_t*)&stmDerivativePtr_,
            36,
            DataPort::eDouble
        }
    };
}

errc_t BlockStateTransitionMatrix::run(const SimTime& simTime)
{
    // dΦ/dt = A · Φ
    //
    // A: 动力学雅可比矩阵 ∂f/∂x（6×6）
    // Φ: 状态转换矩阵（6×6）
    //
    // 使用 MatrixMN 泛型乘法：MatrixMN<I,J> × MatrixMN<J,K> → MatrixMN<I,K>

    const Matrix6d& A      = *aMatrixPtr_;
    const Matrix6d& Phi    = *stmPtr_;
    Matrix6d&       dPhidt = *stmDerivativePtr_;

    dPhidt = A * Phi;

    return eNoError;
}

AST_NAMESPACE_END
