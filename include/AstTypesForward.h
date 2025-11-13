#pragma once
#include "AstMacro.h"

typedef int err_t;


AST_NAMESPACE_BEGIN

template<typename Scalar, size_t N>
class VectorN;

template<typename Scalar, size_t M, size_t N>
class MatrixMN;

typedef VectorN<double, 3> Vector3d;

typedef MatrixMN<double, 3, 3> Matrix3d;

AST_NAMESPACE_END