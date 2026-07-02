#include "AstUtil/Constants.h"
#include <cmath>

#pragma once
#ifndef SWIG

namespace KReal
{
    constexpr double PI = _AST kPI;
}

#ifdef AST_BUILD_LIB
inline double power(const double x, const double y)
{
    return std::pow(x, y);
}
#endif

#endif