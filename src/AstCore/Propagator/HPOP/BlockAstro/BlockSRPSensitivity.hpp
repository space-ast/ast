///
/// @file      BlockSRPSensitivity.hpp
/// @brief     SRP综合参数K敏感度传播函数块
/// @details   计算状态对SRP综合参数K的敏感度向量 Ψ_K = ∂[r,v]/∂K 的时间导数
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

#pragma once

#include "AstGlobal.h"
#include "AstCore/BlockDerivative.hpp"
#include "AstMath/Vector.hpp"
#include "AstMath/Matrix.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup BlockAstro
    @{
*/

/// @brief SRP综合参数K敏感度传播函数块
/// @details
/// 计算 Ψ_K = ∂[r,v]/∂K 的时间导数：
///   dΨ_K/dt = A(t) · Ψ_K + [0, 0, 0, ∂a_srp/∂K]ᵀ
///
/// 其中：
///   Ψ_K = 状态对SRP综合参数 K = Cr·A/m 的偏导数（6×1）
///   A(t) = 动力学雅可比矩阵 ∂f/∂x（6×6）
///   ∂a_srp/∂K = a_srp / K（3维加速度向量，由 BlockSRPPartial 提供）
///
/// 输入端口：
///   - "AMatrix"：动力学雅可比矩阵 A(t)（36维，6×6 行优先存储）
///   - "StateSensitivityToSRP"：敏感度向量 Ψ_K（6维）
///   - "AccSensitivityToSRP"：SRP加速度对K的偏导数（3维，由 BlockSRPPartial 提供）
///
/// 导数端口：
///   - "StateSensitivityToSRP"：dΨ_K/dt（6维）
///
/// @note 本块读取 AMatrix 输入端口，拓扑排序保证在所有力模型块之后执行
/// @see BlockSRPPartial, BlockStateTransitionMatrix
class AST_CORE_API BlockSRPSensitivity : public BlockDerivative
{
public:
    BlockSRPSensitivity();
    ~BlockSRPSensitivity() override = default;

    errc_t run(const SimTime& simTime) override;

private:
    void init();

    Matrix6d* aMatrixPtr_{&aMatrixBuffer_};             ///< A 矩阵输入（6×6）
    Matrix6d  aMatrixBuffer_{};                         ///< A 矩阵缓冲区

    double* stateSensitivityToSRPPtr_{stateSensBuffer_};           ///< Ψ_K 输入（6维）
    double* accSensitivityToSRPPtr_{accSensBuffer_};               ///< 强迫项输入 ∂a_srp/∂K（3维）
    double* stateSensitivityToSRPDerivativePtr_{stateSensDerivBuffer_}; ///< dΨ_K/dt 输出（6维）

    double stateSensBuffer_[6]{};         ///< Ψ_K 缓冲区
    double accSensBuffer_[3]{};           ///< 强迫项缓冲区
    double stateSensDerivBuffer_[6]{};    ///< dΨ_K/dt 缓冲区
};

/*! @} */

AST_NAMESPACE_END
