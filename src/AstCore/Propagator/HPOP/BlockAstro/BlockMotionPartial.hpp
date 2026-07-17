///
/// @file      BlockMotionPartial.hpp
/// @brief     运动学偏导数函数块
/// @details   在 BlockMotion 基础上向 A 矩阵写入位置运动学偏导数 ∂(dr/dt)/∂v = I
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
#include "BlockMotion.hpp"
#include "AstMath/Matrix.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup BlockAstro
    @{
*/

/// @brief 运动学偏导数函数块
/// @details
/// 继承 BlockMotion，在其基础上向 A 矩阵写入位置运动学偏导数。
/// 重写 run()：计算 dr/dt = v 的同时，将 ∂(dr/dt)/∂v = I 写入 A 矩阵。
///
/// A 矩阵布局（6×6，行优先）：
///           位置导数与位置无关，∂(dr/dt)/∂r = 0（无需写入）
///         ↓ 本块写入 I (rows 0-2, cols 3-5)
///   ┌         ┐
///   │ 0   I   │  rows 0-2: 位置运动学
///   │ ∂a/∂r ∂a/∂v │  rows 3-5: 加速度偏导数
///   └         ┘
///
/// 输入端口（继承）：
///   - "Vel"：速度向量（3维，继承自 BlockMotion）
///
/// 输出端口（新增）：
///   - "AMatrix"：动力学雅可比矩阵 A(t)（36维，本块写入 ∂(dr/dt)/∂v = I）
///
/// 导数端口（继承）：
///   - "Pos"：位置导数（3维），赋值为当前速度
///
/// @note 可替换 BlockMotion 使用，提供偏导数计算的同时保持原有运动学关系
/// @see BlockMotion, BlockStateTransitionMatrix
class AST_CORE_API BlockMotionPartial : public BlockMotion
{
public:
    BlockMotionPartial();

    BlockMotionPartial(const BlockMotionPartial&) = delete;
    BlockMotionPartial& operator=(const BlockMotionPartial&) = delete;

    errc_t run(const SimTime& simTime) override;
private:
    void init();
private:
    Matrix6d* aMatrixPtr_{&aMatrixBuffer_};   ///< A 矩阵指针（6×6）
    Matrix6d  aMatrixBuffer_{};               ///< A 矩阵缓冲区
};

/*! @} */

AST_NAMESPACE_END
