///
/// @file      BlockSRP.hpp
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
#include "AstMath/Vector.hpp"
#include "AstCore/ForceModel.hpp"       // for ESunPosition
#include "AstCore/EclipseCalculator.hpp"

AST_NAMESPACE_BEGIN

class CelestialBody;
class Frame;
class SimTime;


/// @brief 1 AU处的太阳辐射压力 [N/m²]，使用太阳光度计算得到
/// @details
/// 由太阳光度推导：
///   Φ(1AU) = L_sun / (4π · AU²)          — 1AU处的辐射通量 [W/m²]
///   P_1AU  = Φ(1AU) / c                  — 1AU处的辐射压力 [N/m²]
///          = L_sun / (4π · AU² · c)
///
/// 代入：
///   L_sun = 3.839×10²⁶ W (Carrol & Ostlie, 2007)
///   AU    = 1.49597870691×10¹¹ m
///   c     = 2.99792458×10⁸ m/s
///
///   P_1AU ≈ 4.553×10⁻⁶ N/m²
constexpr double kSolarPressureAt1AU_FromLuminosity = kSunLuminosity / (4.0 * kPI * kAU * kAU * kLightSpeed);

/// @brief 1 AU处的太阳辐射压力 [N/m²]，使用太阳常数计算得到
/// @details
///   P_1AU = Φ(1AU) / c ≈ 1367 / 2.99792458e8 ≈ 4.560e-6 N/m²
constexpr double kSolarPressureAt1AU_FromSolarConstant = kSolarConstant / kLightSpeed;

/// @brief 1 AU处的太阳辐射压力 [N/m²]，目前使用光度计算的值，也可以切换为使用太阳常数计算的值
constexpr double kSolarPressureAt1AU = kSolarPressureAt1AU_FromLuminosity;  



/// @brief 太阳辐射压力函数块
/// @details
/// 计算太阳辐射压力加速度，添加到速度导数上。
/// 计算公式：
///   a_srp = Cr * (A/m) * P_1AU * (AU/r)^2 * u_sun2sc
/// 其中：
///   Cr  = 光压系数（辐射压力系数）
///   A   = SRP面积（航天器截面积）
///   m   = 航天器质量
///   P_1AU = 1AU处的太阳辐射压力（~4.56e-6 N/m^2）
///   AU  = 天文单位
///   r   = 太阳到航天器的距离
///   u_sun2sc = 太阳指向航天器的单位方向向量 
/// 
class AST_CORE_API BlockSRP : public BlockDerivative
{
public:
    BlockSRP();

    BlockSRP(const BlockSRP&) = delete;
    BlockSRP& operator=(const BlockSRP&) = delete;

    /// @param sun 太阳
    /// @param cr 光压系数
    /// @param srpArea SRP面积
    /// @param propagationFrame 预报坐标系
    /// @note 构造函数这里只保留必需的参数，其他额外的配置参数通过set方法设置
    BlockSRP(EclipseCalculator* eclipseCalculator, double cr, double srpArea, Frame* propagationFrame);

    ~BlockSRP() override;

    errc_t run(const SimTime& simTime) override;
public:
    void setSunPosition(ESunPosition sunPosition){sunPosition_ = sunPosition;}
protected:
    Vector3d* position_{&vectorBuffer_};            ///< 位置（以主要天体为参考，预报坐标系下）
    Vector3d* velocity_{&vectorBuffer_};            ///< 速度（以主要天体为参考，预报坐标系下）
    Vector3d* accSRP_{&vectorBuffer_};              ///< 太阳辐射压力加速度
    Vector3d* velocityDerivative_{&vectorBuffer_};  ///< 速度导数
    double* mass_{&doubleBuffer_};                  ///< 航天器质量
    double doubleBuffer_{};                         ///< 浮点数缓冲区
    Vector3d vectorBuffer_{};                       ///< 向量缓冲区

protected:
    EclipseCalculator* eclipseCalculator_{};        ///< 阴影光照计算器
    double cr_{1.0};                                ///< 光压系数
    double srpArea_{20.0};                          ///< SRP面积 [m^2]
    Frame* propagationFrame_{};                     ///< 预报坐标系
    ESunPosition sunPosition_{ESunPosition::eTrue}; ///< 太阳位置
};

AST_NAMESPACE_END
