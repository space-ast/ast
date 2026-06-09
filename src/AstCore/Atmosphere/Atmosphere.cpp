///
/// @file      Atmosphere.cpp
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

#include "Atmosphere.hpp"
#include "AstUtil/Logger.hpp"
#include <cmath>

AST_NAMESPACE_BEGIN

/*
table

Kp = 0o   0+   1-   1o   1+   2-   2o   2+   3-   3o   3+   4-   4o   4+

ap =  0    2    3    4    5    6    7    9   12   15   18   22   27   32



Kp = 5-   5o   5+   6-   6o   6+   7-   7o   7+   8-   8o   8+   9-   9o

ap = 39   48   56   67   80   94  111  132  154  179  207  236  300  400

*/


double aKpToAp(double kp)
{
    double r = 28.0 * kp + 0.03 * exp(kp) - 100.0;
    /*
    newton: 
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


#if 1
double aApToKp(double ap)
{
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

#else

/// @note 这个问题，牛顿法没有割线法好
/// @param ap 
/// @return 
double aApToKp(double ap)
{
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

#endif

AST_NAMESPACE_END
