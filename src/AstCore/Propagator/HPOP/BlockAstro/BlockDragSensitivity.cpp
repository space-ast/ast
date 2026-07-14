///
/// @file      BlockDragSensitivity.cpp
/// @brief     弹道系数B敏感度传播函数块实现
/// @details   实现 dΨ_B/dt = A(t)·Ψ_B + [0,0,0,∂a_drag/∂B]ᵀ
/// @author    axel
/// @date      2026-07-14
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

#include "BlockDragSensitivity.hpp"
#include "AstCore/BlockAstro.hpp"
#include "AstUtil/IdentifierAPI.hpp"
#include "AstMath/Vector.hpp"
#include "AstMath/Matrix.hpp"
#include "AstMath/MathOperator.hpp"

AST_NAMESPACE_BEGIN

BlockDragSensitivity::BlockDragSensitivity()
    : BlockDerivative{}
{
    init();
}

void BlockDragSensitivity::init()
{
    static auto identifierAMatrix = aIdentifier(kIdentifierAMatrix);
    static auto identStateSensDrag = aIdentifier(kIdentifierStateSensitivityToDrag);
    static auto identAccSensDrag = aIdentifier(kIdentifierAccSensitivityToDrag);

    inputPorts_ = {
        {identifierAMatrix, (signal_t*)&aMatrixPtr_, 36, DataPort::eDouble},
        {identStateSensDrag, (signal_t*)&stateSensitivityToDragPtr_, 6, DataPort::eDouble},
        {identAccSensDrag, (signal_t*)&accSensitivityToDragPtr_, 3, DataPort::eDouble},
    };

    derivativePorts_ = {
        {identStateSensDrag, (signal_t*)&stateSensitivityToDragDerivativePtr_, 6, DataPort::eDouble},
    };
}

errc_t BlockDragSensitivity::run(const SimTime& simTime)
{
    // dΨ_B/dt = A · Ψ_B + [0, 0, 0, ∂a_drag/∂B]ᵀ
    const Matrix6d& A = *aMatrixPtr_;
    const double* psi = stateSensitivityToDragPtr_;
    double* dpsi = stateSensitivityToDragDerivativePtr_;

    // 齐次部分: A · Ψ_B
    for (int i = 0; i < 6; ++i) {
        double sum = 0;
        for (int j = 0; j < 6; ++j)
            sum += A(i, j) * psi[j];
        dpsi[i] = sum;
    }

    // 强迫项: ∂a_drag/∂B 写入 rows 3-5
    dpsi[3] += accSensitivityToDragPtr_[0];
    dpsi[4] += accSensitivityToDragPtr_[1];
    dpsi[5] += accSensitivityToDragPtr_[2];

    return eNoError;
}

AST_NAMESPACE_END
