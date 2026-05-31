#pragma once

// for ifort in windows
// #define MSISE90_FUNCNAME(low, UP) UP
typedef int integer;
typedef double real;

#ifdef __cplusplus
extern "C"{
#endif

    // f2c
    /*
    该函数不能并行计算
    stl: LOCAL APPARENT SOLAR TIME(HRS)
    void gtd6_(int* year, double* sec, double* alt, double* glat, double* glong, double* stl, double* f107a, double* f107, double* ap, int* mass, double* d, double* t);
    */
    int  gtd6_(integer *iyd, real *sec, real *alt,  real *glat,   real *glong,   real *stl,   real *f107a,   real *f107,   const real *ap, integer *mass, real *d, real *t);

#ifdef __cplusplus
}
#endif