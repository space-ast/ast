---
title: Mathematical Operations
description: Comprehensive mathematical functions including vector operations, matrix multiplication, and trigonometric functions
---

# Mathematical Operations Module

## Overview

The Mathematical Operations module provides a comprehensive set of mathematical functions for vector and matrix operations. The module is designed to support multiple container types while maintaining type safety and performance.

## Supported Operations

### Basic Mathematical Functions
- Sign function, trigonometric extensions (cot, acot)
- Floating-point precision calculations (eps)

### Vector Operations
- **Dot Product**: `dot()`, `dot3()` (optimized for 3D vectors)
- **Cross Product**: `cross()` for 3D vectors
- **Vector Norm**: `norm()` for magnitude calculation
- **Normalization**: `normalize()` (in-place) and `normalized()` (copy)

### Arithmetic Operations
- Scalar-vector operations: `scalar + vector`, `vector * scalar`, etc.
- Vector-vector operations: `vector + vector`, `vector - vector`

### Matrix Multiplication
- MatrixMN class multiplication
- Native 2D array multiplication using `mtimes()`

## Multi-container Support

The module is designed to support various data container types:

- Standard library containers: `std::vector`, `std::array`
- Native C arrays: `double[N]`
- Native pointers: `double*`
- Any custom container supporting `size()` and subscript access `[]`

## Usage Examples

### Basic Vector Operations

```cpp
#include "AstCore/MathOperator.hpp"

using namespace AST_NAMESPACE;

// Using std::vector
std::vector<double> v1 = {1, 2, 3};
std::vector<double> v2 = {4, 5, 6};

double dot_product = dot(v1, v2);           // Dot product
auto cross_product = cross(v1, v2);         // Cross product
double magnitude = norm(v1);               // Norm
auto normalized_v = normalized(v1);         // Normalization

// Using native arrays
double arr1[3] = {1, 2, 3};
double arr2[3] = {4, 5, 6};

dot_product = dot(arr1, arr2);
cross_product = cross(arr1, arr2);
```

### Arithmetic Operations

```cpp
std::vector<double> v = {1, 2, 3};

// Scalar operations
auto v2 = 2.0 * v;      // {2, 4, 6}
auto v3 = v + v;        // {2, 4, 6}
auto v4 = v - 1.0;      // {0, 1, 2}
```

### Matrix Operations

```cpp
// Using MatrixMN class
MatrixMN<double, 2, 3> A = {...};
MatrixMN<double, 3, 2> B = {...};
auto C = A * B;  // 2x2 matrix

// Using native arrays
double A[2][3] = {{1,2,3}, {4,5,6}};
double B[3][2] = {{7,8}, {9,10}, {11,12}};
auto C = mtimes(A, B);  // 2x2 matrix
```

## Design Philosophy

### Template-based Design

The module uses template functions that work with any type supporting standardized interfaces (`size()` and subscript access `[]`). This design provides maximum flexibility while maintaining type safety.

### Performance Considerations

- Specialized functions for known dimensions (e.g., `dot3`, `cross3`) to avoid loop overhead
- Support for both in-place operations (`normalize`) and copy operations (`normalized`)
- Compile-time size checking for native array versions

### Error Handling

- `assert` checks for vector size consistency
- `static_assert` for compile-time array size checking
- Safe handling of zero-vector normalization

## API Reference

### Basic Mathematical Functions

```cpp
// Sign function
template<typename _Scalar>
int sign(_Scalar val)

// Trigonometric extensions
double cot(double a)
double acot(double a)

// Floating-point precision calculation
double eps(double t)
```

### Vector Operations

#### Dot Product

```cpp
// Container version
template<typename Container1, typename Container2>
double dot(const Container1& vec1, const Container2& vec2)

// Native array version
template<size_t N1, size_t N2>
double dot(const double(&vec1)[N1], const double(&vec2)[N2])

// Pointer version
double dot(const double* vec1, const double* vec2, size_t N)
double dot3(const double* vec1, const double* vec2)  // Optimized for 3D vectors
```

#### Cross Product

```cpp
// 3D vector cross product (container version)
template<typename Vector3D1, typename Vector3D2>
auto cross(const Vector3D1& vec1, const Vector3D2& vec2)

// Pointer version
void cross(double* res, const double* vec1, const double* vec2)

// Native array version
template<size_t N1, size_t N2>
std::array<double, 3> cross(const double (&vec1)[N1], const double (&vec2)[N2])
```

#### Vector Norm

```cpp
// Pointer version
double norm(double* vec, size_t N)

// Container version
template<typename Vector>
double norm(const Vector& vec)

// Native array version
template <size_t N>
double norm(const double (&vec)[N])
```

#### Normalization

```cpp
// In-place normalization
void normalize(double* vec, size_t N)
template<typename Vector> void normalize(Vector& vec)
template <size_t N> void normalize(double (&vec)[N])

// Return normalized copy
template<typename Vector> Vector normalized(const Vector& vec)
template <size_t N> std::array<double, N> normalized(double (&vec)[N])
template <size_t N> std::array<double, N> normalized(double* vec)
```

### Arithmetic Operators

The module automatically generates arithmetic operators for supported container types:

```cpp
// Vector-scalar operations
vector = scalar + vector
vector = vector + scalar  
vector = scalar - vector
vector = vector - scalar
vector = scalar * vector
vector = vector * scalar
vector = scalar / vector
vector = vector / scalar

// Vector-vector operations
vector = vector + vector
vector = vector - vector
```

### Matrix Multiplication

```cpp
// MatrixMN class matrix multiplication
template<typename _Scalar, size_t I, size_t J, size_t K>
MatrixMN<_Scalar, I, K> operator*(const MatrixMN<_Scalar, I, J>& left, 
                                  const MatrixMN<_Scalar, J, K>& right)

// Native 2D array matrix multiplication
template<typename _Scalar, size_t I, size_t J, size_t K>
std::array<std::array<_Scalar, K>, I> mtimes(const _Scalar(&left)[I][J], 
                                             const _Scalar(&right)[J][K])
```

## Dependencies

- `AstGlobal.hpp`: Project global definitions
- `math.h`: Mathematical functions
- `assert.h`: Assertion checking
- `type_traits`: Type traits
- `array`: Standard array container