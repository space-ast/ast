#pragma once
#include "AstGlobal.h"
#include <stdio.h>
#include <math.h>

#ifndef SWIG

A_CLINKAGE_BEGIN

/// @brief Vinti 解析解的闭式轨道预报（含 J2 / J3 扁率项）。
/// @param planet[4] 行星参数 {ae, gm, J2, J3}
///                  ae = 赤道半径 [km]，gm = 引力常数 [km^3/s^2]，
///                  J2 / J3 = 带谐重力系数。
/// @param vt0       初始时刻 [s]
/// @param x0[6]     初始状态，位置与速度，ECI [km, km/s]
/// @param vt1       终止时刻 [s]
/// @param x1[6]     [out] 终态，位置与速度，ECI [km, km/s]
/// @param oe[6]     [out] Vinti 平均根数：
/// @note  输入输出使用公制导出单位 (km, km/s)；内部归一化到天文单位以减小舍入误差。
/// @note  当 |vt1 - vt0| < 1e-15 时直接返回 x1 = x0；距地球小于 210 km 时直接返回。
AST_CORE_CAPI void Vinti6 (double planet[4], double vt0, double x0[6], double vt1, double x1[6], double oe[6]);

void Kepler1 (double planet[4], double t0, double x0[6], double t1, double x1[6], double *xxx);

double hmod360(double angle);

A_CLINKAGE_END

#endif
