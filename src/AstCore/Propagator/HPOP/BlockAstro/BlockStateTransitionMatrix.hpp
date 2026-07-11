///
/// @file      BlockStateTransitionMatrix.hpp
/// @brief     状态转换矩阵（STM）函数块
/// @details   计算状态转换矩阵 Φ(t) 的时间导数：dΦ/dt = A(t) · Φ(t)
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

#pragma once

#include "AstGlobal.h"
#include "BlockDerivative.hpp"
#include "AstMath/Matrix.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup BlockAstro
    @{
*/

/// @brief 状态转换矩阵（STM）函数块
/// @details
/// 计算状态转换矩阵 Φ(t) 的时间导数，用于线性化误差传播。
///
/// 微分方程：
///   dΦ/dt = A(t) · Φ(t),   Φ(0) = I
///
/// 其中：
///   Φ = 状态转换矩阵（6×6），将初始状态偏差映射到当前时刻
///   A(t) = 动力学雅可比矩阵 ∂f/∂x（6×6）
///
/// A 矩阵结构（6维状态：位置3 + 速度3）：
///   ┌             ┐
///   │   0     I   │   上左 3×3：零矩阵（位置导数不直接依赖位置）
///   │ ∂a/∂r ∂a/∂v │   上右 3×3：单位矩阵（位置对速度的导数 = 速度）
///   └             ┘   下左 3×3：加速度对位置的偏导数
///                     下右 3×3：加速度对速度的偏导数
///
/// 工作流程：
///   1. 各力模型块（BlockTwoBody、BlockGravity、BlockDrag 等）计算各自的
///      加速度梯度（∂a_i/∂r, ∂a_i/∂v），累加到 A 矩阵中
///   2. 本块读取组装好的 A 矩阵和当前状态转换矩阵 Φ
///   3. 计算 dΦ/dt = A · Φ，输出到导数端口供积分器使用
///
/// 输入端口：
///   - "AMatrix"：动力学雅可比矩阵 A(t)（36维，6×6 行优先存储）
///
/// 输出端口：
///   - "StateTransitionMatrix"：状态转换矩阵 Φ（36维，6×6 行优先存储）
///     @note 该端口信号会被 BlockDynamicSystem 重定向到状态向量，
///           积分器负责更新 Φ 的值，本块仅读取 Φ 用于计算其导数
///
/// 导数端口：
///   - "StateTransitionMatrix"：Φ 的时间导数 dΦ/dt（36维，6×6 行优先存储）
///
/// @note Φ(0) = I 的初始化由调用方在设置初始状态向量时完成
/// @see BlockDynamicSystem, BlockAstro.hpp
class AST_CORE_API BlockStateTransitionMatrix : public BlockDerivative
{
public:
    BlockStateTransitionMatrix();
    ~BlockStateTransitionMatrix() override = default;

    errc_t run(const SimTime& simTime) override;

private:
    void init();

    Matrix6d* aMatrixPtr_{buffer_};           ///< A 矩阵输入指针（6×6）
    Matrix6d* stmPtr_{buffer_ + 1};           ///< 状态转换矩阵 Φ 指针（6×6）
    Matrix6d* stmDerivativePtr_{buffer_ + 2}; ///< Φ 导数指针 dΦ/dt（6×6）
    Matrix6d  buffer_[3]{};                   ///< 矩阵缓冲区（A、Φ、dΦ/dt）
};


/*! @} */

AST_NAMESPACE_END
