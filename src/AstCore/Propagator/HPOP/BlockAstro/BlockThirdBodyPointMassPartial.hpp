///
/// @file      BlockThirdBodyPointMassPartial.hpp
/// @brief     三体点质量引力偏导数（梯度）函数块
/// @details   在 BlockThirdBodyPointMass 基础上增加点质量引力梯度计算，直接写入 A 矩阵
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
#include "AstCore/BlockThirdBodyPointMass.hpp"
#include "AstMath/Matrix.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup BlockAstro
    @{
*/

/// @brief 三体点质量引力偏导数（梯度）函数块
/// @details
/// 继承 BlockThirdBodyPointMass，在其基础上增加三体点质量引力梯度计算。
/// 重写 run()：调用基类加速度计算，同时计算梯度张量，
/// 将 ∂a_3body/∂r 累加到 A 矩阵的 bottom-left 3×3 位置。
///
/// 三体点质量引力加速度：
///   a = GM₃ · [ (r₃ - r) / |r₃ - r|³  -  r₃ / |r₃|³ ]
///
/// 梯度（对航天器位置 r 的偏导数）：
///   间接项梯度 = 0（与 r 无关）
///   直接项梯度 ∂a_direct/∂r = -GM₃/|d|³ · I + 3·GM₃/|d|⁵ · (d ⊗ d)
///   其中 d = r₃ - r，该形式与 aPointMassField 的 Hessian 完全相同
///
/// A 矩阵布局（6×6，行优先）：
///   ┌         ┐
///   │ 0   I   │  rows 0-2: 位置运动学
///   │ ∂a/∂r 0 │  rows 3-5: 加速度偏导数
///   └         ┘
///       ↑ 本块写入 ∂a/∂r (rows 3-5, cols 0-2)
///         三体引力与速度无关，∂a/∂v = 0
///
/// 输入端口（继承）：
///   - "Pos"：位置向量（3维，继承自 BlockThirdBodyPointMass）
///
/// 输出端口（继承 + 新增）：
///   - "AccThirdBody"：三体引力加速度（3维，继承自 BlockThirdBodyPointMass）
///   - "AMatrix"：动力学雅可比矩阵 A(t)（36维，新增，本块累加 ∂a/∂r 分量）
///
/// 导数端口（继承）：
///   - "Vel"：速度导数（3维），累加三体引力加速度
///
/// @note 可替换 BlockThirdBodyPointMass 使用，提供梯度计算的同时保持原有加速度输出
/// @see BlockThirdBodyPointMass, BlockStateTransitionMatrix
class AST_CORE_API BlockThirdBodyPointMassPartial : public BlockThirdBodyPointMass
{
public:
    BlockThirdBodyPointMassPartial();
    explicit BlockThirdBodyPointMassPartial(Point* thirdBody, double thirdBodyGM, Frame* propagationFrame);

    errc_t run(const SimTime& simTime) override;
private:
    void init();

    Matrix6d* aMatrixPtr_{&aMatrixBuffer_};   ///< A 矩阵指针（6×6）
    Matrix6d  aMatrixBuffer_{};               ///< A 矩阵缓冲区
};

/*! @} */

AST_NAMESPACE_END
