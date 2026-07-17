///
/// @file      BlockDrag.hpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2026-01-19
/// @copyright 版权所有 (C) 2026-present, ast项目.
///
/// ast项目（https://github.com/space-ast/ast）
/// 本项目基于 Apache 2.0 开源许可证分发。
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
#include "AstCore/Atmosphere.hpp"
#include "AstMath/Vector.hpp"

AST_NAMESPACE_BEGIN

/// @brief 大气阻力函数块
/// @details
/// 计算大气阻力加速度，用于低轨道航天器的大气阻力摄动建模。
///
/// 方程：
///   a_drag = -½ · Cd · (A/m) · ρ · |v_rel| · v_rel
///
/// 其中：
///   Cd    = 阻力系数（dragCoefficient_）
///   A     = 阻力参考面积（dragArea_）
///   m     = 航天器质量（来自 Mass 输入端口）
///   ρ     = 大气密度（由大气模型根据位置计算）
///   v_rel = 航天器相对于大气的速度（已减去大气旋转速度）
///
/// 输入端口：
///   - "Pos"：位置向量（3维，预报坐标系下）
///   - "Vel"：速度向量（3维，预报坐标系下）
///   - "Mass"：航天器质量（标量）
///
/// 输出端口：
///   - "AccDrag"：大气阻力加速度（3维）
///
/// 导数端口：
///   - "Vel"：速度导数（3维），累加阻力加速度
///
/// @note atmosphere_ 的所有权由本块管理，析构时自动释放
class AST_CORE_API BlockDrag: public BlockDerivative
{
public:
    BlockDrag();

    BlockDrag(const BlockDrag&) = delete;
    BlockDrag& operator=(const BlockDrag&) = delete;

    BlockDrag(Atmosphere* atmosphere, double dragCoefficient, double dragArea, Frame* propagationFrame);

    ~BlockDrag() override;

    errc_t run(const SimTime& simTime) override;

protected:
    Vector3d* position_{&vectorBuffer_};             ///< 位置（预报坐标系下）
    Vector3d* velocity_{&vectorBuffer_};             ///< 速度（预报坐标系下）
    Vector3d* accDrag_{&vectorBuffer_};              ///< 阻力加速度
    Vector3d* velocityDerivative_{&vectorBuffer_};   ///< 速度导数
    double* mass_{&doubleBuffer_};                   ///< 质量
    double doubleBuffer_{};                          ///< 浮点数缓冲区
    Vector3d vectorBuffer_{};                        ///< 向量缓冲区
protected:
    Atmosphere* atmosphere_{};                       ///< 大气模型
    double dragCoefficient_{};                       ///< 阻力系数
    double dragArea_{};                              ///< 阻力面积
    Frame* propagationFrame_{};                      ///< 预报坐标系
};


AST_NAMESPACE_END
