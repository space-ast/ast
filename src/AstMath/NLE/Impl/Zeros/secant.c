///
/// @file      secant.c
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2026-01-27
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

#include <math.h>
#include "zeros.h"

double
secant(callback_type f, double xa, double xb, double xtol, double rtol,
       int iter, void *func_data_param, scipy_zeros_info *solver_stats)
{
    int i;
    double dm,xm,fm,fa,fb;
    solver_stats->error_num = INPROGRESS;

    fa = (*f)(xa, func_data_param);
    fb = (*f)(xb, func_data_param);
    solver_stats->funcalls = 2;
    if (fa == 0) {
        solver_stats->error_num = CONVERGED;
        return xa;
    }
    if (fb == 0) {
        solver_stats->error_num = CONVERGED;
        return xb;
    }
    if (signbit(fa)==signbit(fb)) {
        solver_stats->error_num = SIGNERR;
        return 0.;
    }
    solver_stats->iterations = 0;
    for (i=0; i<iter; i++) {
        solver_stats->iterations++;
        // dm = fa*(xb - xa)/(fb - fa);
        // xm = xa - dm;
        // xm = (xa*fb - xb*fa)/(fb - fa);
        if(fabs(fb) > fabs(fa))
        {
            xm = (-fa/fb*xb + xa) / (1 - fa/fb);
        }else{
            xm = (-fb/fa*xa + xb) / (1 - fb/fa);
        }
        fm = (*f)(xm, func_data_param);
        solver_stats->funcalls++;
        if (signbit(fm)==signbit(fb)) {
            xb = xm; fb = fm;
        }else{
            xa = xm; fa = fm;
        }
        dm = xb - xa;
        if (fm == 0 || fabs(dm) < xtol || fabs(dm) < rtol*fabs(xm)) {
            solver_stats->error_num = CONVERGED;
            return xm;
        }
    }
    solver_stats->error_num = CONVERR;
    return xa;
}

