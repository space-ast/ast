///
/// @file      BlockAstro.hpp
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
#include "AstCore/FuncBlock.hpp"

AST_NAMESPACE_BEGIN

/// @name 信号标识符常量
/// @brief 用于函数块之间端口自动连接的信号名称标识符
/// @details
/// 每个常量对应一个信号通道，BlockDynamicSystem 通过匹配输入/输出端口的标识符名称，
/// 自动建立函数块之间的信号连接。新增力模型时，需在此定义其加速度信号的标识符。
/// @{

constexpr const char* kIdentifierPos = "Pos";                       ///< 位置信号（预报坐标系下，3维向量）
constexpr const char* kIdentifierVel = "Vel";                       ///< 速度信号（预报坐标系下，3维向量）
constexpr const char* kIdentifierMass = "Mass";                     ///< 质量信号（标量）

constexpr const char* kIdentifierSTM = "StateTransitionMatrix";     ///< 状态转换矩阵信号
constexpr const char* kIdentifierAMatrix = "AMatrix";               ///< 动力学的雅可比矩阵 A(t) = ∂f/∂x, f = dx/dt; 
                                                                    ///< Φ为状态转移矩阵， x(t) = Φ(t)·x₀， dΦ/dt = A(t) · Φ(t)

/*
          位置(x, y, z)   速度(vx, vy, vz)
位置导数    [   0    ]      [   I    ]    
速度导数    [ ∂a/∂r  ]      [ ∂a/∂v  ]    
*/

constexpr const char* kIdentifierAcc = "Acc";                       ///< 总加速度信号（3维向量）
constexpr const char* kIdentifierAccTwoBody = "AccTwoBody";         ///< 二体引力加速度（3维向量）
constexpr const char* kIdentifierAccThirdBody = "AccThirdBody";     ///< 三体引力加速度（3维向量）
constexpr const char* kIdentifierAccGravity = "AccGravity";         ///< 重力场加速度（3维向量）
constexpr const char* kIdentifierAccGravityPert = "AccGravityPert"; ///< 重力场摄动加速度（3维向量）
constexpr const char* kIdentifierAccRelativity = "AccRelativity";   ///< 相对论修正加速度（3维向量）
constexpr const char* kIdentifierAccDrag = "AccDrag";               ///< 大气阻力加速度（3维向量）
constexpr const char* kIdentifierAccSRP = "AccSRP";                 ///< 太阳辐射压力加速度（3维向量）

/// @}

/// @brief 天体力学函数块类型别名（待细化）
using BlockAstro = FuncBlock; // @todo;


AST_NAMESPACE_END
