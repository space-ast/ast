#pragma once

// for ifort in windows
#define USSA76_FUNCNAME(low, UP) ATMOSPHERE1976_mp_##UP

// for gfortran in windows 
// #define USSA76_FUNCNAME(low, UP) __atmosphere1976_MOD_##low

#ifdef __cplusplus
extern "C"{
#endif

#define Atmosphere USSA76_FUNCNAME(atmosphere, ATMOSPHERE)

void USSA76_FUNCNAME(upperatmosphere, UPPERATMOSPHERE)
(const double* alt, double* sigma, double* delta, double* theta);


void USSA76_FUNCNAME(loweratmosphere, LOWERATMOSPHERE)
(const double* alt, double* sigma, double* delta, double* theta);


void USSA76_FUNCNAME(simpleatmosphere, SIMPLEATMOSPHERE)
(const double* alt, double* sigma, double* delta, double* theta);


void USSA76_FUNCNAME(atmosphere, ATMOSPHERE)
(const double* alt, double* sigma, double* delta, double* theta);


#ifdef __cplusplus
}
#endif