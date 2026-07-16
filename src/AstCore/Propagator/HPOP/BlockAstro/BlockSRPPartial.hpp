///
/// @file      BlockSRPPartial.hpp
/// @brief     太阳辐射压力偏导数（梯度）函数块
/// @details   在 BlockSRP 基础上增加 SRP 加速度偏导数计算，直接写入 A 矩阵
/// @author    axel
/// @date      2026-07-12
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
#include "AstCore/BlockSRP.hpp"
#include "AstMath/Matrix.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

/// @brief 太阳辐射压力偏导数（梯度）函数块
/// @details
/// 继承 BlockSRP，在其基础上增加 SRP 加速度偏导数计算。
/// 重写 run()：计算 SRP 加速度的同时计算雅可比矩阵，
/// 将 ∂a_srp/∂r 累加到 A 矩阵对应位置。
///
/// SRP 加速度：
///   a_srp = -ν · Cr · (A/m) · P_1AU · (AU/r)² · u_sun2sc
///         = β/r³ · scToSun
///   其中 β = -ν · Cr · A/m · P_1AU · AU²
///
/// 偏导数：
///   ∂a_srp/∂r = factor · (3·scToSun·scToSunᵀ/r² − I₃)
///   ∂a_srp/∂v = 0
///   其中 factor = β/r³（即 run() 中已计算的标量因子）
///
/// A 矩阵布局（6×6，行优先）：
///   ┌         ┐
///   │ 0   I   │  rows 0-2: 位置运动学
///   │ ∂a/∂r 0 │  rows 3-5: 加速度偏导数
///   └         ┘
///       ↑ 本块写入 ∂a_srp/∂r (rows 3-5, cols 0-2)
///         SRP 与速度无关，∂a_srp/∂v = 0
///
/// 输入端口（继承）：
///   - "Pos"：位置向量（3维，继承自 BlockSRP）
///   - "Vel"：速度向量（3维，继承自 BlockSRP）
///   - "Mass"：航天器质量（标量，继承自 BlockSRP）
///
/// 输出端口（继承 + 新增）：
///   - "AccSRP"：太阳辐射压力加速度（3维，继承自 BlockSRP）
///   - "AMatrix"：动力学雅可比矩阵 A(t)（36维，新增，本块累加 ∂a_srp/∂r 分量）
///
/// 导数端口（继承）：
///   - "Vel"：速度导数（3维），累加 SRP 加速度
///
/// @note 可替换 BlockSRP 使用，提供偏导数计算的同时保持原有加速度输出
/// @see BlockSRP, BlockStateTransitionMatrix
class AST_CORE_API BlockSRPPartial : public BlockSRP
{
public:
    BlockSRPPartial();
    BlockSRPPartial(EclipseCalculator* eclipseCalculator, double cr, double srpArea, Frame* propagationFrame);

    errc_t run(const SimTime& simTime) override;

    /// @brief 设置是否启用SRP综合参数K敏感度传播
    void setUseSRPSensitivity(bool v) { useSRPSensitivity_ = v; }
private:
    void init();

    Matrix6d* aMatrixPtr_{&aMatrixBuffer_};   ///< A 矩阵指针（6×6）
    Matrix6d  aMatrixBuffer_{};               ///< A 矩阵缓冲区

    bool      useSRPSensitivity_{false};                              ///< 是否启用SRP综合参数K敏感度传播
    Vector3d* accSensitivityToSRP_{&accSensitivityToSRPBuffer_};     ///< ∂a_srp/∂K 强迫项输出
    Vector3d  accSensitivityToSRPBuffer_{};                           ///< 强迫项缓冲区
};

/*! @} */

AST_NAMESPACE_END
