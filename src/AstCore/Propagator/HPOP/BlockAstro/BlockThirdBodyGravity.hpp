///
/// @file      BlockThirdBodyGravity.hpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2026-06-24
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
#include "AstCore/BlockAstro.hpp"
#include "AstCore/BlockDerivative.hpp"
#include "AstCore/GravityCalculator.hpp"
#include "AstCore/Axes.hpp"
#include "AstMath/Vector.hpp"

AST_NAMESPACE_BEGIN

/// @brief 三体重力场函数块
/// @details
/// 计算第三天体的球谐展开重力场摄动加速度（直接引力与间接引力之差）。
///
/// 工作流程：
///   1. 获取第三天体在预报系下的位置 r₃
///   2. 计算航天器相对于第三天体的位置: r_rel = r - r₃
///   3. 将 r_rel 旋转变换到第三天体的重力场固连坐标系
///   4. 调用 GravityCalculator 计算固连系下的总重力加速度（含中心项 + 摄动项）
///   5. 将加速度逆变换回报系（得到直接项 a_direct）
///   6. 计算间接项：-r₃ 变换到重力固连系，同样调用 GravityCalculator 计算总加速度再逆变换（得到 a_indirect）
///   7. 总摄动: a_total = a_direct - a_indirect
///   8. 累加到速度导数
///
/// 方程：
///   a = a_direct(r - r₃)  -  a_indirect(-r₃)
///
/// 其中：
///   a_direct(r - r₃)   = GravityCalculator::calcTotalAcceleration(r - r₃)
///                         航天器在第三天体引力场中的总加速度（含中心项与摄动项）
///   a_indirect(-r₃)     = GravityCalculator::calcTotalAcceleration(-r₃)
///                         中心天体在第三天体引力场中的总加速度（含中心项与摄动项）
/// 当 degree=0 时两项退化为点质量公式，与 BlockThirdBodyPointMass 结果一致。
///
/// 输入端口：
///   - "Pos"：位置向量（3维，预报坐标系下，以中心天体为参考）
///
/// 输出端口：
///   - "AccThirdBody"：三体引力加速度（3维）
///
/// 导数端口：
///   - "Vel"：速度导数（3维），累加三体引力加速度
///
/// @see BlockThirdBodyPointMass, BlockGravity
class AST_CORE_API BlockThirdBodyGravity : public BlockDerivative
{
public:
    BlockThirdBodyGravity();
    explicit BlockThirdBodyGravity(Point* thirdBody, GravityField&& gravityField,
                                   int degree, int order, Axes* gravityAxes,
                                   Frame* propagationFrame);

    errc_t run(const SimTime& simTime) override;

    /// @brief 设置是否考虑重力场系数长期变化
    void setConsiderVariations(bool considerVariations) { considerVariations_ = considerVariations; }
protected:
    Vector3d* posCBI{&vectorBuffer};                          ///< 位置（以中心天体为参考）
    Vector3d* accThirdBody{&vectorBuffer};                    ///< 三体重力加速度（输出）
    Vector3d* velocityDerivative_{&vectorBuffer};             ///< 速度导数
    Vector3d vectorBuffer{};                                  ///< 向量缓冲区
private:
    void init();
protected:
    Point* thirdBody_{};                              ///< 三体
    HAxes  gravityAxes_;                              ///< 三体重力场固连系
    HAxes  propagationAxes_;                          ///< 预报系轴系
    Frame* propagationFrame_{};                       ///< 预报系（用于获取三体位置）
    GravityCalculatorDefault gravityCalculator_;      ///< 重力计算器
    bool   considerVariations_{false};                ///< 是否考虑重力场系数长期变化
};

AST_NAMESPACE_END
