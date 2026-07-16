///
/// @file      BlockThirdBodyGravityPartial.hpp
/// @brief     三体球谐重力偏导数（梯度）函数块
/// @details   在 BlockThirdBodyGravity 基础上增加球谐重力梯度计算，直接写入 A 矩阵
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
#include "AstCore/BlockThirdBodyGravity.hpp"
#include "AstMath/Matrix.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup BlockAstro
    @{
*/

/// @brief 三体球谐重力偏导数（梯度）函数块
/// @details
/// 继承 BlockThirdBodyGravity，在其基础上增加球谐重力梯度计算。
/// 重写 run()：在第三体固连系中调用 calcTotalAcceleration 的梯度版本，
/// 通过相似变换将梯度张量变换回传播系，
/// 将 ∂a_3body/∂r 累加到 A 矩阵的 bottom-left 3×3 位置。
///
/// 三体球谐重力加速度：
///   a_total = a_direct(r - r₃) - a_indirect(-r₃)
///
/// 梯度（对航天器位置 r 的偏导数）：
///   间接项梯度 = 0（-r₃ 与 r 无关）
///   直接项梯度：在第三体固连系中计算 H_grav = ∂a_direct/∂(r_rel)，
///   然后通过相似变换 H_prop = Rᵀ · H_grav · R 变换到传播系
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
///   - "Pos"：位置向量（3维，继承自 BlockThirdBodyGravity）
///
/// 输出端口（继承 + 新增）：
///   - "AccThirdBody"：三体引力加速度（3维，继承自 BlockThirdBodyGravity）
///   - "AMatrix"：动力学雅可比矩阵 A(t)（36维，新增，本块累加 ∂a/∂r 分量）
///
/// 导数端口（继承）：
///   - "Vel"：速度导数（3维），累加三体引力加速度
///
/// @note 可替换 BlockThirdBodyGravity 使用，提供梯度计算的同时保持原有加速度输出。
///       与 BlockThirdBodyPointMassPartial 不同，本块采用完全重写 run() 的策略
///       （而非调基类后补梯度），以避免重复昂贵的球谐重力计算。
/// @see BlockThirdBodyGravity, BlockGravityPartial, BlockStateTransitionMatrix
class AST_CORE_API BlockThirdBodyGravityPartial : public BlockThirdBodyGravity
{
public:
    BlockThirdBodyGravityPartial();

    BlockThirdBodyGravityPartial(const BlockThirdBodyGravityPartial&) = delete;
    BlockThirdBodyGravityPartial& operator=(const BlockThirdBodyGravityPartial&) = delete;

    explicit BlockThirdBodyGravityPartial(Point* thirdBody, GravityField&& gravityField,
                                          int degree, int order, Axes* gravityAxes,
                                          Frame* propagationFrame);

    errc_t run(const SimTime& simTime) override;

    /// @brief 设置计算偏导数时使用的重力场阶数（可小于加速度计算阶数以节省计算）
    void setDegreeForPartial(int degree) { gravityCalculator_.setDegreeForPartial(degree); }

    /// @brief 设置计算偏导数时使用的重力场次数（可小于加速度计算次数以节省计算）
    void setOrderForPartial(int order) { gravityCalculator_.setOrderForPartial(order); }
private:
    void init();

    Matrix6d* aMatrixPtr_{&aMatrixBuffer_};   ///< A 矩阵指针（6×6）
    Matrix6d  aMatrixBuffer_{};               ///< A 矩阵缓冲区
};

/*! @} */

AST_NAMESPACE_END
