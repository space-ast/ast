/* Written by Charles Harris charles.harris@sdl.usu.edu */

/* Modified to not depend on Python everywhere by Travis Oliphant.
 */

/*
 * Modified to be used in ast project.
 */

#include "AstGlobal.h"
#include "AstMath/SolverStats.h"

#ifndef ZEROS_H
#define ZEROS_H

// typedef struct {
//     int funcalls;
//     int iterations;
//     int error_num;
// } scipy_zeros_info;

typedef struct _AST SolverStats scipy_zeros_info;


/* Must agree with _ECONVERGED, _ESIGNERR, _ECONVERR  in zeros.py */
#define CONVERGED 0
#define SIGNERR -1
#define CONVERR -2
#define EVALUEERR -3
#define INPROGRESS 1

typedef double (*callback_type)(double, void*);
typedef double (*solver_type)(callback_type, double, double, double, double,
                              int, void *, scipy_zeros_info*);

AST_MATH_CAPI double bisect(callback_type f, double xa, double xb, double xtol,
                     double rtol, int iter, void *func_data_param,
                     scipy_zeros_info *solver_stats);
AST_MATH_CAPI double ridder(callback_type f, double xa, double xb, double xtol,
                     double rtol, int iter, void *func_data_param,
                     scipy_zeros_info *solver_stats);
AST_MATH_CAPI double brenth(callback_type f, double xa, double xb, double xtol,
                     double rtol, int iter, void *func_data_param,
                     scipy_zeros_info *solver_stats);
AST_MATH_CAPI double brentq(callback_type f, double xa, double xb, double xtol,
                     double rtol, int iter, void *func_data_param,
                     scipy_zeros_info *solver_stats);
AST_MATH_CAPI double secant(double(*func)(double, void*), double xa, double xb, double xtol,
                     double rtol, int iter, void *func_data_param,
                     scipy_zeros_info *solver_stats);

#endif
