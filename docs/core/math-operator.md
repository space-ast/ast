# MathOperator 数学运算功能

*说明：本文档旨在帮助理解模块功能，具体API和行为请查阅源代码。*

## 概述

`MathOperator.hpp` 是 ast 项目中的数学运算工具模块，提供了一系列通用的向量和矩阵运算函数。

该模块采用模板化设计，支持多种数据容器类型，包括标准库容器、原生数组和指针等。

## 功能特性

### 支持的运算类型

- **基本数学函数**: `sign`, `cot`, `acot`, `eps`
- **向量运算**: 
  - 点积 (`dot`)
  - 叉积 (`cross`) 
  - 范数 (`norm`)
  - 归一化 (`normalize`, `normalized`)
- **算术运算符**: 支持向量与标量的加减乘除运算
- **矩阵乘法**: 支持固定大小矩阵的乘法运算

### 多容器支持

模块设计为支持多种数据容器类型：

- `std::vector`, `std::array` 等标准库容器
- 原生 C 数组 `double[N]`
- 原生指针 `double*`
- 任何支持 `size()` 和下标访问 `[]` 的自定义容器


## 使用示例

### 基本向量运算

```cpp
#include "AstCore/MathOperator.hpp"

using namespace AST_NAMESPACE;

// 使用 std::vector
std::vector<double> v1 = {1, 2, 3};
std::vector<double> v2 = {4, 5, 6};

double dot_product = dot(v1, v2);           // 点积
auto cross_product = cross(v1, v2);         // 叉积
double magnitude = norm(v1);                // 范数
auto normalized_v = normalized(v1);         // 归一化

// 使用原生数组
double arr1[3] = {1, 2, 3};
double arr2[3] = {4, 5, 6};

dot_product = dot(arr1, arr2);
cross_product = cross(arr1, arr2);
```

### 算术运算

```cpp
std::vector<double> v = {1, 2, 3};

// 标量运算
auto v2 = 2.0 * v;      // {2, 4, 6}
auto v3 = v + v;        // {2, 4, 6}
auto v4 = v - 1.0;      // {0, 1, 2}
```

### 矩阵运算

```cpp
// 使用 MatrixMN 类
MatrixMN<double, 2, 3> A = {...};
MatrixMN<double, 3, 2> B = {...};
auto C = A * B;  // 2x2 矩阵

// 使用原生数组
double A[2][3] = {{1,2,3}, {4,5,6}};
double B[3][2] = {{7,8}, {9,10}, {11,12}};
auto C = mtimes(A, B);  // 2x2 矩阵
```


## 设计说明

### 模板化设计

模块采用模板函数实现，只要类型支持标准化的接口（`size()` 和下标访问 `[]`）即可使用这些函数。这种设计提供了极大的灵活性，同时保持类型安全。

### 性能考虑

- 对于已知维度的运算（如 `dot3`, `cross3`），提供专用函数以避免循环开销
- 支持原地操作 (`normalize`) 和返回副本的操作 (`normalized`)
- 编译时尺寸检查用于原生数组版本

### 错误处理

- 使用 `assert` 检查向量尺寸一致性
- 使用 `static_assert` 在编译时检查数组尺寸
- 对零向量归一化进行安全处理


## API 参考

### 基本数学函数

```cpp
// 符号函数
template<typename _Scalar>
int sign(_Scalar val)

// 三角函数扩展
double cot(double a)
double acot(double a)

// 浮点数精度计算
double eps(double t)
```

### 向量运算

#### 点积 (Dot Product)

```cpp
// 容器版本
template<typename Container1, typename Container2>
double dot(const Container1& vec1, const Container2& vec2)

// 原生数组版本
template<size_t N1, size_t N2>
double dot(const double(&vec1)[N1], const double(&vec2)[N2])

// 指针版本
double dot(const double* vec1, const double* vec2, size_t N)
double dot3(const double* vec1, const double* vec2)  // 专为3维向量优化
```

#### 叉积 (Cross Product)

```cpp
// 3维向量叉积（容器版本）
template<typename Vector3D1, typename Vector3D2>
auto cross(const Vector3D1& vec1, const Vector3D2& vec2)

// 指针版本
void cross(double* res, const double* vec1, const double* vec2)

// 原生数组版本
template<size_t N1, size_t N2>
std::array<double, 3> cross(const double (&vec1)[N1], const double (&vec2)[N2])
```

#### 范数 (Norm)

```cpp
// 指针版本
double norm(double* vec, size_t N)

// 容器版本
template<typename Vector>
double norm(const Vector& vec)

// 原生数组版本
template <size_t N>
double norm(const double (&vec)[N])
```

#### 归一化 (Normalization)

```cpp
// 原地归一化
void normalize(double* vec, size_t N)
template<typename Vector> void normalize(Vector& vec)
template <size_t N> void normalize(double (&vec)[N])

// 返回归一化副本
template<typename Vector> Vector normalized(const Vector& vec)
template <size_t N> std::array<double, N> normalized(double (&vec)[N])
template <size_t N> std::array<double, N> normalized(double* vec)
```

### 算术运算符

模块自动为支持的容器类型生成算术运算符：

```cpp
// 向量与标量运算
vector = scalar + vector
vector = vector + scalar  
vector = scalar - vector
vector = vector - scalar
vector = scalar * vector
vector = vector * scalar
vector = scalar / vector
vector = vector / scalar

// 向量与向量运算
vector = vector + vector
vector = vector - vector
```

### 矩阵乘法

```cpp
// MatrixMN 类矩阵乘法
template<typename _Scalar, size_t I, size_t J, size_t K>
MatrixMN<_Scalar, I, K> operator*(const MatrixMN<_Scalar, I, J>& left, 
                                  const MatrixMN<_Scalar, J, K>& right)

// 原生二维数组矩阵乘法
template<typename _Scalar, size_t I, size_t J, size_t K>
std::array<std::array<_Scalar, K>, I> mtimes(const _Scalar(&left)[I][J], 
                                             const _Scalar(&right)[J][K])
```

## 依赖关系

- `AstGlobal.h`: 项目全局定义
- `math.h`: 数学函数
- `assert.h`: 断言检查
- `type_traits`: 类型特征
- `array`: 标准数组容器

