///
/// @file      GeomagneticIndex.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-06-09
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

#include "GeomagneticIndex.hpp"
#include "AstUtil/Logger.hpp"
#include <cmath>

AST_NAMESPACE_BEGIN



double aKpToAp_TableLookup(const double kp)
{
    // 参考GMAT
    // 通过查表将kp转换为ap, table 8-3 from Vallado,  3rd edition
    int index = (int)((kp + .01) * 3);
    switch (index)
    {
       case 0:
          return 0.0;
       case 1:
          return 2.0;
       case 2:
          return 3.0;
       case 3:
          return 4.0;
       case 4:
          return 5.0;
       case 5:
          return 6.0;
       case 6:
          return 7.0;
       case 7:
          return 9.0;
       case 8:
          return 12.0;
       /// case 9 is the default, below
       case 10:
          return 18.0;
       case 11:
          return 22.0;
       case 12:
          return 27.0;
       case 13:
          return 32.0;
       case 14:
          return 39.0;
       case 15:
          return 48.0;
       case 16:
          return 56.0;
       case 17:
          return 67.0;
       case 18:
          return 80.0;
       case 19:
          return 94.0;
       case 20:
          return 111.0;
       case 21:
          return 132.0;
       case 22:
          return 154.0;
       case 23:
          return 179.0;
       case 24:
          return 207.0;
       case 25:
          return 236.0;
       case 26:
          return 300.0;
       case 27:
          return 400.0;
       case 9:
       default:
          return 15.0;
    }
    return 15.0;
}


double aKpToAp_ExponentialApproximation(const double kp)
{
    // 参考GMAT
    // 通过指数近似将kp转换为ap, Vallado 2nd Edition, (eq 8-31)
    return exp((kp + 1.6) / 1.75);
}


/*
table

Kp = 0o   0+   1-   1o   1+   2-   2o   2+   3-   3o   3+   4-   4o   4+

ap =  0    2    3    4    5    6    7    9   12   15   18   22   27   32



Kp = 5-   5o   5+   6-   6o   6+   7-   7o   7+   8-   8o   8+   9-   9o

ap = 39   48   56   67   80   94  111  132  154  179  207  236  300  400

*/

double aKpToAp_SecantMethod(double kp)
{
    /*
    割线法求解关系式
    \f[28 K_p + 0.03 e^{K_p} = A_p + 100 (1 - e^{-0.08 A_p})\f]
    */
    double r = 28.0 * kp + 0.03 * exp(kp) - 100.0;
    double x[3], y[2];
    x[0] = 0.0;
    x[1] = 500.0;

    const double  epsilon = 1.0e-6;
    const int maxIterations = 15;
    int i = 0;
    do
    {
        y[0] = 100.0 * exp(-0.08 * x[0]) + r - x[0];
        y[1] = 100.0 * exp(-0.08 * x[1]) + r - x[1];

        x[2] = x[1] - y[1] * (x[1] - x[0]) / (y[1] - y[0]);
        x[0] = x[1];
        x[1] = x[2];

        if (i++ > maxIterations) {
            aError("max iterations exceeded");
            return x[2];
        }
    } while (fabs(y[1]) > epsilon);
    return x[2];
}



double aKpToAp_NewtonMethod(double kp)
{
    /*
    牛顿法求解关系式
    \f[28 K_p + 0.03 e^{K_p} = A_p + 100 (1 - e^{-0.08 A_p})\f]
    */

    double r = 28.0 * kp + 0.03 * exp(kp) - 100.0;
    /*
    牛顿法迭代：
        y  = 100.0 * exp(-0.08 * x) + r - x
        y' = -8 * exp(-0.08 * x) - 1
        x  = x0 - y/y'
    */
    double x = exp((kp + 1.6) / 1.75);  // approx

    const double  epsilon = 1.0e-6;
    const int maxIterations = 15;
    int i = 0;
    double temp, dx;
    do
    {
        temp = exp(-0.08 * x);
        dx   = (100.0 * temp + r - x) / (-8 * temp - 1);
        // double y    = 100.0 * exp(-0.08 * x) + r - x;
        // double ydot = -8 * exp(-0.08 * x) -1;
        x -= dx;

        if (i++ > maxIterations) {
            aError("max iterations exceeded");
            return x;
        }
    } while (fabs(dx) > epsilon);
    return x;
}


double aApToKp_SecantMethod(double ap)
{
    /*
    割线法求解关系式
    \f[28 K_p + 0.03 e^{K_p} = A_p + 100 (1 - e^{-0.08 A_p})\f]
    */
    double r = -100.0 * exp(-0.08 * ap) + 100 + ap; //28.0 * kp + 0.03 * exp(kp) - 100.0;
    double x[3], y[2];
    x[0] = 0.0;
    x[1] = 10;	//@todo: enough?

    const double  epsilon = 1.0e-8;
    const int maxIterations = 20;
    int i = 0;
    do
    {
        y[0] = -28.0 * x[0] - 0.03 * exp(x[0]) + r;   //100.0 * exp(-0.08 * x[0]) + r - x[0];
        y[1] = -28.0 * x[1] - 0.03 * exp(x[1]) + r;   //100.0 * exp(-0.08 * x[1]) + r - x[1];

        x[2] = x[1] - y[1] * (x[1] - x[0]) / (y[1] - y[0]);
        x[0] = x[1];
        x[1] = x[2];

        if (i++ > maxIterations) {
            aError("max iterations exceeded");
            return x[2];
        }
    } while (fabs(y[1]) > epsilon);
    return x[2];
}


double aApToKp_NewtonMethod(double ap)
{
    /*
    牛顿法求解关系式
    \f[28 K_p + 0.03 e^{K_p} = A_p + 100 (1 - e^{-0.08 A_p})\f]
    */

    // note 这个问题，牛顿法没有割线法好
    
    double r = -100.0 * exp(-0.08 * ap) + 100 + ap; //28.0 * kp + 0.03 * exp(kp) - 100.0;
    double x = 6;
    
    const double  epsilon = 1.0e-14;
    const int maxIterations = 30;
    int i = 0;
    double temp, dx;
    do
    {
        /*
            y  =  -28*x - 0.03 * exp(x) + r;
            y' = - 28 - 0.03*exp(x);
        */
        temp = -0.03*exp(x);
        dx = ( -28 * x + temp + r)/(-28 + temp);
        x -= dx;
        
        if (i++ > maxIterations) {
            aError("AsGeomagApToKp failed; too many iterations");
            return x;
        }
    } while (fabs(dx) > epsilon);
    return x;
}



double aApToKp_LinearInterpolate(double ap)
{
    /*
    线性插值转换
    */
    double kp = 0.0;
    double apl, apr, kpl, kpr;
 
    if (ap <= 2.0)
    {
        apl = 0.0;      apr = 2.0;
        kpl = 0.0;      kpr = 0.33;
    }
    else if (ap <= 3.0)
    {
        apl = 2.0;      apr = 3.0;
        kpl = 0.33;     kpr = 0.67;
    }
    else if (ap <= 4.0)
    {
        apl = 3.0;      apr = 4.0;
        kpl = 0.67;     kpr = 1.0;
    }
    else if (ap <= 5.0)
    {
        apl = 4.0;      apr = 5.0;
        kpl = 1.0;      kpr = 1.33;
    }
    else if (ap <= 6.0)
    {
        apl = 5.0;      apr = 6.0;
        kpl = 1.33;     kpr = 1.67;
    }
    else if (ap <= 7.0)
    {
        apl = 6.0;      apr = 7.0;
        kpl = 1.67;     kpr = 2.0;
    }
    else if (ap <= 9.0)
    {
        apl = 7.0;      apr = 9.0;
        kpl = 2.0;      kpr = 2.33;
    }
    else if (ap <= 12.0)
    {
        apl = 9.0;      apr = 12.0;
        kpl = 2.33;     kpr = 2.67;
    }
    else if (ap <= 15.0)
    {
        apl = 12.0;     apr = 15.0;
        kpl = 2.67;     kpr = 3.0;
    }
    else if (ap <= 18.0)
    {
        apl = 15.0;     apr = 18.0;
        kpl = 3.0;      kpr = 3.33;
    }
    else if (ap <= 22.0)
    {
        apl = 18.0;     apr = 22.0;
        kpl = 3.33;     kpr = 3.67;
    }
    else if (ap <= 27.0)
    {
        apl = 22.0;     apr = 27.0;
        kpl = 3.67;     kpr = 4.0;
    }
    else if (ap <= 32.0)
    {
        apl = 27.0;     apr = 32.0;
        kpl = 4.0;      kpr = 4.33;
    }
    else if (ap <= 39.0)
    {
        apl = 32.0;     apr = 39.0;
        kpl = 4.33;     kpr = 4.67;
    }
    else if (ap <= 48.0)
    {
        apl = 39.0;     apr = 48.0;
        kpl = 4.67;     kpr = 5.0;
    }
    else if (ap <= 56.0)
    {
        apl = 48.0;     apr = 56.0;
        kpl = 5.0;      kpr = 5.33;
    }
    else if (ap <= 67.0)
    {
        apl = 56.0;     apr = 67.0;
        kpl = 5.33;     kpr = 5.67;
    }
    else if (ap <= 80.0)
    {
        apl = 67.0;     apr = 80.0;
        kpl = 5.67;     kpr = 6.0;
    }
    else if (ap <= 94.0)
    {
        apl = 80.0;     apr = 94.0;
        kpl = 6.0;      kpr = 6.33;
    }
    else if (ap <= 111.0)
    {
        apl = 94.0;     apr = 111.0;
        kpl = 6.33;     kpr = 6.67;
    }
    else if (ap <= 132.0)
    {
        apl = 111.0;    apr = 132.0;
        kpl = 6.67;     kpr = 7.0;
    }
    else if (ap <= 154.0)
    {
        apl = 132.0;    apr = 154.0;
        kpl = 7.0;      kpr = 7.33;
    }
    else if (ap <= 179.0)
    {
        apl = 154.0;    apr = 179.0;
        kpl = 7.33;     kpr = 7.67;
    }
    else if (ap <= 207.0)
    {
        apl = 179.0;    apr = 207.0;
        kpl = 7.67;     kpr = 8.0;
    }
    else if (ap <= 236.0)
    {
        apl = 207.0;    apr = 236.0;
        kpl = 8.0;      kpr = 8.33;
    }
    else if (ap <= 300.0)
    {
        apl = 236.0;    apr = 300.0;
        kpl = 8.33;     kpr = 8.67;
    }
    else
    {
        apl = 300.0;    apr = 400.0;
        kpl = 8.67;     kpr = 9.0;
    }
 
    kp = kpl + (ap - apl) * (kpr - kpl) / (apr - apl);
 
    return kp;
}


double aKpToAp(double kp)
{
    return aKpToAp_TableLookup(kp);
}


double aApToKp(double ap)
{
    return aApToKp_LinearInterpolate(ap);
}


AST_NAMESPACE_END
