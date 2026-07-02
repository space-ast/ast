///
/// @file      atmos76.cpp
/// @brief     ussa1976 大气模型
/// @details   
/// @author    axel
/// @date      2026-06-26
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

#include "atmos76.hpp"
#include <cmath>
// 该源文件 atmos76.cpp 是根据原代码 tables.cpp 和 atmos76.f90 所进行的实现

AST_NAMESPACE_BEGIN



// 美国标准大气1976 物理常数
const double REARTH = 6356.766;    // 地球极半径 (km)
const double GMR    = 34.163195;   // 流体静力常数, kelvins/km
const double TZERO  = 288.15;      // 海平面温度, K

// 私有辅助函数的前向声明
static double EvaluateCubic(double a, double fa, double fpa,
                            double b, double fb, double fpb, double u);
static double KineticTemperature(double z);


void LowerAtmosphere(
    const double  alt,    // 几何高度, km
    double& sigma,        // 密度/海平面标准密度
    double& delta,        // 压力/海平面标准压力
    double& theta)        // 温度/海平面标准温度
// 计算标准大气中的温度、密度和压力
// 精确到 86 km，超过此高度仅为近似值。
{
  const int NTAB = 8;
  int i,j,k;

  static double htab[NTAB] = {0.0,  11.0, 20.0, 32.0, 47.0,
                 51.0, 71.0, 84.852 };
  static double ttab[NTAB] = { 288.15, 216.65, 216.65, 228.65, 270.65,
                  270.65, 214.65, 186.946 };
  static double ptab[NTAB] = { 1.0, 2.2336110E-1, 5.4032950E-2, 8.5666784E-3,
     1.0945601E-3, 6.6063531E-4, 3.9046834E-5, 3.68501E-6 };
  static double gtab[NTAB] = { -6.5, 0, 1.0, 2.8, 0, -2.8, -2.0, 0 };

  double h=alt*REARTH/(alt+REARTH);     // 几何高度转换为位势高度

  i=0; j=NTAB-1;  // 二分查找初始值
  do
    {
      k=(i+j)/2;
      if (h < htab[k]) j=k; else i=k;
    }  while (j > i+1);

  double tgrad=gtab[i];                      // 本层温度梯度
  double tbase=ttab[i];                      // 本层基准温度
  double deltah=h-htab[i];                   // 距本层基准的高度
  double tlocal=tbase+tgrad*deltah;          // 本地温度
  theta=tlocal/ttab[0];                                  // 温度比

  if (0.0 == tgrad)                                         // 压力比
    delta=ptab[i]*exp(-GMR*deltah/tbase);
  else
    delta=ptab[i]*pow(tbase/tlocal, GMR/tgrad);

  sigma=delta/theta;                                        // 密度比
}   // ------------------------------------------- End of function LowerAtmosphere


// 计算由两点处的函数值和一阶导数定义的三次多项式。
// 通常 a < u < b，但非必须。
static double EvaluateCubic(double a, double fa, double fpa,
                            double b, double fb, double fpb, double u)
{
    double d = (fb - fa) / (b - a);
    double t = (u - a) / (b - a);
    double p = 1.0 - t;
    return p*fa + t*fb - p*t*(b - a)*(p*(d - fpa) - t*(d - fpb));
}


// 计算 86 km 以上的动力学温度。
static double KineticTemperature(double z)
{
    // 表5 - 86 km 至 1000 km 动力学温度定义
    const double C1 = -76.3232;   // 文档中的大写 A
    const double C2 =  19.9429;   // 文档中的小写 a
    const double C3 =  12.0;
    const double C4 =   0.01875;  // 文档中的 lambda
    const double TC = 263.1905;
    const double T7 = 186.8673;
    const double Z8 =  91.0;
    const double Z9 = 110.0, T9  = 240.0;
    const double Z10 = 120.0, T10 = 360.0;
    const double T12 = 1000.0;    // 即 T 无穷

    if (z <= Z8) {
        return T7;                                // 式 (25), p.11
    } else if (z < Z9) {
        double xx = (z - Z8) / C2;                // 来自附录 B, p.223
        double yy = sqrt(1.0 - xx*xx);
        return TC + C1*yy;                        // 式 (27), p.11
    } else if (z <= Z10) {
        return T9 + C3*(z - Z9);                  // 式 (29)
    } else {
        double xx = (REARTH + Z10) / (REARTH + z);
        double yy = (T12 - T10) * exp(-C4*(z - Z10)*xx);
        return T12 - yy;                          // 式 (31)
    }
}


// 计算 1976 标准大气从 86 km 到 1000 km 的属性。
void UpperAtmosphere(
    const double  alt,    // 几何高度, km
    double& sigma,        // 密度/海平面标准密度
    double& delta,        // 压力/海平面标准压力
    double& theta)        // 温度/海平面标准温度
{
    const int NTABLE = 25;

    // 高度表 (km)
    static const double Z[NTABLE] = {
        86.0,  93.0, 100.0, 107.0, 114.0,
       121.0, 128.0, 135.0, 142.0, 150.0,
       160.0, 170.0, 180.0, 190.0, 200.0,
       220.0, 260.0, 300.0, 400.0, 500.0,
       600.0, 700.0, 800.0, 900.0,1000.0
    };

    // 压力表 (DELTA = P/P0)
    static const double DELTA_TABLE[NTABLE] = {
        3.6850E-6, 1.0660E-6, 3.1593E-7, 1.0611E-7, 4.3892E-8,
        2.3095E-8, 1.3997E-8, 9.2345E-9, 6.4440E-9, 4.4828E-9,
        2.9997E-9, 2.0933E-9, 1.5072E-9, 1.1118E-9, 8.3628E-10,
        4.9494E-10, 1.9634E-10, 8.6557E-11, 1.4328E-11, 2.9840E-12,
        8.1056E-13, 3.1491E-13, 1.6813E-13, 1.0731E-13, 7.4155E-14
    };

    // 密度表 (SIGMA = RHO/RHO0)
    static const double SIGMA_TABLE[NTABLE] = {
        5.680E-6, 1.632E-6, 4.575E-7, 1.341E-7, 4.061E-8,
        1.614E-8, 7.932E-9, 4.461E-9, 2.741E-9, 1.694E-9,
        1.007E-9, 6.380E-10, 4.240E-10, 2.923E-10, 2.074E-10,
        1.116E-10, 3.871E-11, 1.564E-11, 2.288E-12, 4.257E-13,
        9.279E-14, 2.506E-14, 9.272E-15, 4.701E-15, 2.907E-15
    };

    // log(delta) 对高度的导数, d(log delta)/dz
    static const double DLOGDELTA[NTABLE] = {
        -0.174061, -0.177924, -0.167029, -0.142755, -0.107859,
        -0.079322, -0.064664, -0.054879, -0.048260, -0.042767,
        -0.037854, -0.034270, -0.031543, -0.029384, -0.027632,
        -0.024980, -0.021559, -0.019557, -0.016735, -0.014530,
        -0.011314, -0.007677, -0.005169, -0.003944, -0.003612
    };

    // log(sigma) 对高度的导数, d(log sigma)/dz
    static const double DLOGSIGMA[NTABLE] = {
        -0.172421, -0.182258, -0.178090, -0.176372, -0.154322,
        -0.113750, -0.090582, -0.075033, -0.064679, -0.056067,
        -0.048461, -0.043042, -0.038869, -0.035648, -0.033063,
        -0.029164, -0.024220, -0.021336, -0.017686, -0.016035,
        -0.014327, -0.011631, -0.008248, -0.005580, -0.004227
    };

    // 捕获超过 1000 km 的高度
    if (alt > Z[NTABLE - 1]) {
        delta = DELTA_TABLE[NTABLE - 1];      // 返回 Z=1000.0 的值
        sigma = SIGMA_TABLE[NTABLE - 1];      //   同上
        theta = 1000.0 / TZERO;
        return;
    }

    // 二分查找包围区间
    int i = 0;
    int j = NTABLE - 1;
    int k;
    do {
        k = (i + j) / 2;
        if (alt < Z[k])
            j = k;
        else
            i = k;
    } while (j > i + 1);

    delta = exp(EvaluateCubic(Z[i], log(DELTA_TABLE[i]), DLOGDELTA[i],
                               Z[i+1], log(DELTA_TABLE[i+1]), DLOGDELTA[i+1], alt));

    sigma = exp(EvaluateCubic(Z[i], log(SIGMA_TABLE[i]), DLOGSIGMA[i],
                               Z[i+1], log(SIGMA_TABLE[i+1]), DLOGSIGMA[i+1], alt));

    theta = KineticTemperature(alt) / TZERO;
}


// 计算美国标准大气1976的特性
void Atmosphere(
    const double  alt,    // 几何高度, km
    double& sigma,        // 密度/海平面标准密度
    double& delta,        // 压力/海平面标准压力
    double& theta)        // 温度/海平面标准温度
{
    if (alt > 86.0)
        UpperAtmosphere(alt, sigma, delta, theta);
    else
        LowerAtmosphere(alt, sigma, delta, theta);
}


AST_NAMESPACE_END
