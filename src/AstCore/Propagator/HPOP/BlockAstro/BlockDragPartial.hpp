///
/// @file      BlockDragPartial.hpp
/// @brief     大气阻力偏导数（梯度）函数块
/// @details   在 BlockDrag 基础上增加阻力加速度偏导数计算，直接写入 A 矩阵
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
#include "AstCore/BlockDrag.hpp"
#include "AstMath/Matrix.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

/// @brief 大气阻力偏导数（梯度）函数块
/// @details
/// 继承 BlockDrag，在其基础上增加阻力加速度偏导数计算。
/// 重写 run()：计算阻力加速度的同时计算雅可比矩阵，
/// 将 ∂a_drag/∂r 和 ∂a_drag/∂v 累加到 A 矩阵对应位置。
///
/// 阻力加速度：
///   a_drag = -½ · Cd · (A/m) · ρ · |v_rel| · v_rel
///
/// A 矩阵布局（6×6，行优先）：
///   ┌         ┐
///   │ 0   I   │  rows 0-2: 位置运动学
///   │ ∂a/∂r ∂a/∂v │  rows 3-5: 加速度偏导数
///   └         ┘
///       ↑                   ↑
///       本块写入 ∂a/∂r      本块写入 ∂a/∂v
///
/// 偏导数公式：
///   定义 D = v_rel·v_relᵀ/|v_rel| + |v_rel|·I,  k = Cd·A/(2m)
///   ∂a/∂v = -k·ρ·D                          （rows 3-5, cols 3-5）
///   ∂a/∂r = k·ρ·D·[ω×] + 密度梯度项         （rows 3-5, cols 0-2）
///
///   [ω×] 为大气参考系相对预报系的角速度叉乘矩阵。
///   密度梯度项已预留接口（computeDensityGradientContribution），待 Atmosphere 接口扩展。
///
/// 输入端口（继承）：
///   - "Pos"：位置向量（3维，继承自 BlockDrag）
///   - "Vel"：速度向量（3维，继承自 BlockDrag）
///   - "Mass"：航天器质量（标量，继承自 BlockDrag）
///
/// 输出端口（继承 + 新增）：
///   - "AccDrag"：大气阻力加速度（3维，继承自 BlockDrag）
///   - "AMatrix"：动力学雅可比矩阵 A(t)（36维，新增，本块累加 ∂a/∂r 和 ∂a/∂v 分量）
///
/// 导数端口（继承）：
///   - "Vel"：速度导数（3维），累加阻力加速度
///
/// @note 可替换 BlockDrag 使用，提供偏导数计算的同时保持原有加速度输出
/// @see BlockDrag, BlockStateTransitionMatrix
class AST_CORE_API BlockDragPartial : public BlockDrag
{
public:
    BlockDragPartial();

    BlockDragPartial(const BlockDragPartial&) = delete;
    BlockDragPartial& operator=(const BlockDragPartial&) = delete;

    BlockDragPartial(Atmosphere* atmosphere, double dragCoefficient, double dragArea, Frame* propagationFrame);

    errc_t run(const SimTime& simTime) override;

    /// @brief 设置是否启用弹道系数B敏感度传播
    void setUseDragSensitivity(bool v) { useDragSensitivity_ = v; }
private:
    void init();

    /// @brief 通过高度有限差分计算密度梯度对 ∂a/∂r 的贡献
    /// @details
    ///   沿径向（高度方向）扰动位置，用前向差分近似 ∂ρ/∂r，
    ///   再通过外积计算对 A 矩阵的贡献。
    ///   密度梯度 ∂ρ/∂r 主要沿径向（高度变化主导），因此一维有限差分已足够。
    /// @param relVelocity 相对速度 v_rel（预报坐标系下）
    /// @param relSpeed 相对速度大小 |v_rel|
    /// @param k 系数 k = Cd·A/(2m)
    /// @param tp 当前时间点
    /// @param posInAtmosFrame 大气坐标系下的位置
    /// @param density 当前位置的大气密度
    /// @param rotToAtmos 从预报坐标系到大气坐标系的旋转矩阵
    /// @return 密度梯度贡献矩阵（3×3，预报坐标系下），需累加到 A(3:5, 0:2)
    Matrix3d computeDensityGradientContribution(const Vector3d& relVelocity, double relSpeed, double k,
        const TimePoint& tp, const Vector3d& posInAtmosFrame, double density, const Matrix3d& rotToAtmos);

    Matrix6d* aMatrixPtr_{&aMatrixBuffer_};   ///< A 矩阵指针（6×6）
    Matrix6d  aMatrixBuffer_{};               ///< A 矩阵缓冲区

    bool      useDragSensitivity_{false};                             ///< 是否启用弹道系数B敏感度传播
    Vector3d* accSensitivityToDrag_{&accSensitivityToDragBuffer_};   ///< ∂a_drag/∂B 强迫项输出
    Vector3d  accSensitivityToDragBuffer_{};                          ///< 强迫项缓冲区
};

/*! @} */

AST_NAMESPACE_END
