///
/// @file      BlockGravityPartial.hpp
/// @brief     重力场偏导数（梯度）函数块
/// @details   在 BlockGravity 基础上增加梯度计算，直接写入 A 矩阵
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
#include "BlockGravity.hpp"
#include "AstMath/Matrix.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

/// @brief 重力场偏导数（梯度）函数块
/// @details
/// 继承 BlockGravity，在其基础上增加重力梯度计算。
/// 重写 run()：调用 calcTotalAcceleration 的梯度版本，
/// 将 ∂a_grav/∂r 累加到 A 矩阵的 bottom-left 3×3 位置。
///
/// A 矩阵布局（6×6，行优先）：
///   ┌         ┐
///   │ 0   I   │  rows 0-2: 位置运动学
///   │ ∂a/∂r ∂a/∂v │  rows 3-5: 加速度偏导数
///   └         ┘
///       ↑ 本块写入 ∂a/∂r (rows 3-5, cols 0-2)
///         重力场的加速度a与速度v无关，无需写入 ∂a/∂v
///
/// 输入端口（继承 + 新增）：
///   - "Pos"：位置向量（3维，继承自 BlockGravity）
///   - "AMatrix"：动力学雅可比矩阵 A(t)（36维，新增，本块累加 ∂a/∂r 分量）
///
/// 输出端口（继承）：
///   - "AccGravity"：重力加速度（3维）
///
/// 导数端口（继承）：
///   - "Vel"：速度导数（3维），累加重力加速度
///
/// @note 可替换 BlockGravity 使用，提供梯度计算的同时保持原有加速度输出
/// @see BlockGravity, BlockStateTransitionMatrix
class AST_CORE_API BlockGravityPartial: public BlockGravity
{
public:
public:
    BlockGravityPartial();
    BlockGravityPartial(const GravityField &gravityField, int degree, int order,
                        Axes* gravityAxes=nullptr, Axes* propagationAxes=nullptr);
    BlockGravityPartial(GravityField &&gravityField, int degree, int order,
                        Axes* gravityAxes=nullptr, Axes* propagationAxes=nullptr);

    void setDegreeForPartial(int degree){this->gravityCalculator_.setDegreeForPartial(degree);}
    
    void setOrderForPartial(int order){this->gravityCalculator_.setOrderForPartial(order);}

    errc_t run(const SimTime& simTime) override;
private:
    void init();
private:
    Matrix6d* aMatrixPtr_{&aMatrixBuffer_};   ///< A 矩阵指针（6×6）
    Matrix6d  aMatrixBuffer_{};               ///< A 矩阵缓冲区
};


/*! @} */

AST_NAMESPACE_END
