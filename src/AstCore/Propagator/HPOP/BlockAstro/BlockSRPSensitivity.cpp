///
/// @file      BlockSRPSensitivity.cpp
/// @brief     SRP综合参数K敏感度传播函数块实现
/// @details   实现 dΨ_K/dt = A(t)·Ψ_K + [0,0,0,∂a_srp/∂K]ᵀ
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

#include "BlockSRPSensitivity.hpp"
#include "AstCore/BlockAstro.hpp"
#include "AstUtil/IdentifierAPI.hpp"
#include "AstMath/Vector.hpp"
#include "AstMath/Matrix.hpp"
#include "AstMath/MathOperator.hpp"

AST_NAMESPACE_BEGIN

BlockSRPSensitivity::BlockSRPSensitivity()
    : BlockDerivative{}
{
    init();
}

void BlockSRPSensitivity::init()
{
    static auto identifierAMatrix = aIdentifier(kIdentifierAMatrix);
    static auto identStateSensSRP = aIdentifier(kIdentifierStateSensitivityWrtSRP);
    static auto identAccSensSRP = aIdentifier(kIdentifierAccSensitivityWrtSRP);

    inputPorts_ = {
        {identifierAMatrix, (signal_t*)&aMatrixPtr_, 36, DataPort::eDouble},
        {identStateSensSRP, (signal_t*)&stateSensitivityToSRPPtr_, 6, DataPort::eDouble},
        {identAccSensSRP, (signal_t*)&accSensitivityToSRPPtr_, 3, DataPort::eDouble},
    };

    derivativePorts_ = {
        {identStateSensSRP, (signal_t*)&stateSensitivityToSRPDerivativePtr_, 6, DataPort::eDouble},
    };
}

errc_t BlockSRPSensitivity::run(const SimTime& simTime)
{
    // dΨ_K/dt = A · Ψ_K + [0, 0, 0, ∂a_srp/∂K]ᵀ
    const Matrix6d& A = *aMatrixPtr_;
    const double* psi = stateSensitivityToSRPPtr_;
    double* dpsi = stateSensitivityToSRPDerivativePtr_;

    // 齐次部分: A · Ψ_K
    for (int i = 0; i < 6; ++i) {
        double sum = 0;
        for (int j = 0; j < 6; ++j)
            sum += A(i, j) * psi[j];
        dpsi[i] = sum;
    }

    // 强迫项: ∂a_srp/∂K 写入 rows 3-5
    dpsi[3] += accSensitivityToSRPPtr_[0];
    dpsi[4] += accSensitivityToSRPPtr_[1];
    dpsi[5] += accSensitivityToSRPPtr_[2];

    return eNoError;
}

AST_NAMESPACE_END
